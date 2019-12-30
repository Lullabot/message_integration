<?php

/**
 * @file
 * Post update hooks for message integration module.
 */

use Drupal\node\Entity\Node;

/**
 * Make sure node authors are all subscribed to their own existing content,
 * skipping blocked users. All active users are automatically subscribed to new
 * content, so we only need to update existing content.
 */
function message_integration_post_update_subscribe_authors(&$sandbox) {

  // Find all content authors.
  $query = \Drupal::entityQuery('node')
    ->condition('status', 1);
  $nids = $query->execute();
  $nodes = Node::loadMultiple($nids);

  // Add a content subscription flag for each owner.
  $flag_service = \Drupal::service('flag');
  $flag_id = 'subscribe_node';
  $flag = $flag_service->getFlagById($flag_id);
  foreach ($nodes as $node) {
    $account = $node->getOwner();
    // Skip inactive users.
    if ($account->isBlocked()) {
      continue;
    }
    // Check if already flagged to avoid exception error.
    $flagging = $flag_service->getFlagging($flag, $node, $account);
    if (!$flagging) {
      $flag_service->flag($flag, $node, $account);
    }
  }
}
