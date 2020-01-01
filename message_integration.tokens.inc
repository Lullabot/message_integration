<?php

/**
 * @file
 * Message tokens to render view modes.
 */

use Drupal\diff\Controller\NodeRevisionController;
use Drupal\Core\Routing\RouteMatch;
use Drupal\node\Entity\Node;

/**
 * Implements hook_token_info().
 *
 * Adds some tokens to generate rendered nodes and comments.
 */
function message_integration_token_info() {
  $type = [
    'name' => t('Message Integration'),
    'description' => t('Tokens for the Custom Message Integration module.'),
    'needs-data' => 'message',
  ];

  // Node related tokens.
  $message['render-node'] = [
    'name' => t("Rendered node"),
    'description' => t("The node rendered to HTML in a view mode, i.e. [render-node:teaser]."),
  ];

  $message['node-diff'] = [
    'name' => t('Node diff'),
    'description' => t('A diff of node changes from previous version.'),
  ];

  // Comment related tokens.
  $message['render-comment'] = [
    'name' => t("Rendered comment"),
    'description' => t("A single comment rendered to HTML."),
  ];

  return [
    'types' => ['message' => $type],
    'tokens' => [
      'message' => $message,
    ],
  ];
}

/**
 * Implements hook_tokens().
 */
function message_integration_tokens($type, $tokens, array $data = [], array $options = []) {
  $replacements = [];

  if ($type == 'message' && !empty($data['message'])) {
    /** @var \Drupal\message\Entity\Message $message */
    $message = $data['message'];
    $comment = NULL;
    $node = NULL;
    if ($message->hasField('field_comment_reference')) {
      $comment = $message->field_comment_reference->entity;
      $node = $comment->getCommentedEntity();
    }
    elseif ($message->hasField('field_node_reference')) {
      $node = $message->field_node_reference->get(0)->entity;
      $original_vid = $new_vid = NULL;
      if ($message->hasField('field_new_vid') && !empty($message->field_new_vid->get(0))) {
        $new_vid = $message->field_new_vid->get(0)->value;
      }
      if ($message->hasField('field_original_vid') && !empty($message->field_original_vid->get(0))) {
        $original_vid = $message->field_original_vid->get(0)->value;
      }
    }

    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'render-comment':
          if (!empty($comment)) {
            $render_array = \Drupal::entityTypeManager()->getViewBuilder('comment')->view($comment);
            $text = \Drupal::service('renderer')->renderRoot($render_array);
            $replacements[$original] = $text;
          }
          break;

        case substr($name, 0, 11) == 'render-node':
          if (!empty($node)) {
            $view_mode = str_replace('render-node:', '', $name);
            if (empty($view_mode)) {
              $render_array = \Drupal::entityTypeManager()->getViewBuilder('node')->view($node);
            }
            else {
              $render_array = \Drupal::entityTypeManager()->getViewBuilder('node')->view($node, $view_mode);
            }
            $text = \Drupal::service('renderer')->renderRoot($render_array);
            $replacements[$original] = $text;
          }
          break;

        case 'node-diff':
          if (!empty($node) && !empty($original_vid) && !empty($new_vid)) {
            $render_array = message_integration_tokens_node_diff($node, $original_vid, $new_vid, 'unified_fields');
            $text = \Drupal::service('renderer')->renderRoot($render_array);
            $replacements[$original] = $text;
          }
          else {
            $replacements[$original] = t('<span class="warning">Unable to display node difference.</span>');
          }
          break;

      }
    }
  }

  return $replacements;
}

/**
 * Helper to show diff between two nodes.
 *
 * @param \Drupal\node\Entity\Node $node
 *   The latest node.
 * @param int $original_vid
 *   The previous vid.
 * @param int $new_vid
 *   The newer vid.
 * @param string $filter
 *   The type of comparison, options include:
 *     - 'unified_fields'
 *     - 'split_fields'
 *     - 'visual_inline'
 *
 * @return array
 *   A render array of the diff between the nodes.
 */
function message_integration_tokens_node_diff(Node $node, $original_vid, $new_vid, $filter) {
  $diffEntityComparison = \Drupal::service('diff.entity_comparison');
  $diffLayoutManager = \Drupal::service('plugin.manager.diff.layout');
  $requestStack = \Drupal::service('request_stack');

  // Create a psuedo route match object for the node url.
  $url = $node->urlInfo();
  $route = Drupal::service('router.route_provider')->getRouteByName($url->getRouteName());
  $route_match = new RouteMatch($url->getRouteName(), $route, ['node' => $node]);

  // Create the same node revision controller used for the revision page.
  $nodeRevisionController = new NodeRevisionController($diffEntityComparison, $diffLayoutManager, $requestStack);

  // Load the original revision and compare it to the newer revision.
  $storage = \Drupal::service('entity_type.manager')->getStorage('node');
  $left = $storage->loadRevision($original_vid);
  $right = $storage->loadRevision($new_vid);
  $render_array = $nodeRevisionController->compareEntityRevisions($route_match, $left, $right, $filter);
  unset($render_array['header']['diff_navigation']);
  unset($render_array['controls']);
  return $render_array;
}
