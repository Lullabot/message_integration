<?php

namespace Drupal\message_integration\Commands;

use Drush\Commands\DrushCommands;
use Drupal\node\Entity\Node;

/**
 * Drush commands.
 *
 * In addition to a commandfile like this one, you need to add a
 * drush.services.yml in root of your module like this module does.
 */
class Commands extends DrushCommands {

  /**
   * Do post installation configuration.
   *
   * @command message_integration:configure
   * @usage message_integration:configure
   *   Ensure all configuration has been completed after installation. This
   *   should only be run AFTER installing all required modules. It includes
   *   steps that can't be done in hook_install() because of race conditions.
   */
  public function setConfiguration() {

    // Make sure all new services are available.
    drupal_flush_all_caches();
    $this->updateConfig();
    // Make sure configuration changes are available.
    drupal_flush_all_caches();
    $this->updateAuthors();
    $this->output()->writeln('Your configuration is complete.');
  }

  /**
   * Configuration update.
   *
   * Set configuration that can't be fixed in update hook when modules are all
   * installed at once.
   */
  protected function updateConfig() {
    $config_factory = \Drupal::configFactory();
    $settings = $config_factory->getEditable('message_subscribe.settings');
    $settings->set('use_queue', TRUE);
    $settings->save(TRUE);

    $settings = $config_factory->getEditable('mailsystem.settings');
    $settings->set('defaults', [
      'sender' => 'swiftmailer',
      'formatter' => 'swiftmailer',
    ]);
    $settings->set('modules', [
      'swiftmailer' => [
        'none' => [
          'formatter' => 'swiftmailer',
          'sender' => 'swiftmailer',
        ],
      ],
      'message_notify' => [
        'none' => [
          'formatter' => 'swiftmailer',
          'sender' => 'swiftmailer',
        ],
      ],
    ]);
    $settings->save(TRUE);

    $settings = $config_factory->getEditable('swiftmailer.message');
    $settings->set('format', 'text/html');
    $settings->set('respect_format', false);
    $settings->save(TRUE);

    $settings = $config_factory->getEditable('flag.flag.subscribe_node');
    $settings->set('status', true);
    $settings->save(TRUE);

    $settings = $config_factory->getEditable('user.role.authenticated');
    $permissions = $settings->get('permissions');
    foreach ([
      'flag subscribe_node',
      'unflag subscribe_node',
    ] as $perm) {
      if (!in_array($perm, $permissions)) {
        $permissions[] = $perm;
      }
    }
    $settings->set('permissions', $permissions);
    $settings->save(TRUE);

  }

  /**
   * Author update.
   *
   * This won't work until all the above configuration has been set. Make sure
   * every author is subscribed to all their existing content.
   */
  protected function updateAuthors() {

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
      // Skip admin and anonymous users.
      if (in_array($account->id(), [0, 1])) {
        continue;
      }
      // Check if already flagged to avoid exception error.
      $flagging = $flag_service->getFlagging($flag, $node, $account);
      if (!$flagging) {
        $flag_service->flag($flag, $node, $account);
      }
    }
  }

}
