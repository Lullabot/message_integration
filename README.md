# Custom Message Implementation

Custom code for Message stack and Swiftmailer to create messages about
 new/changed nodes and comments and email them to subscribers. The following
 modules are required:

- [Message](https://www.drupal.org/project/message)
- [Message Notify](https://www.drupal.org/project/message_notify)
- [Message Subscribe](https://www.drupal.org/project/message_subscribe)
- [Message UI](https://www.drupal.org/project/message_ui)
- [Mailsystem](https://www.drupal.org/project/mailsystem)
- [Swiftmailer](https://www.drupal.org/project/swiftmailer)


## To use:

Add the Lullabot repository to your composer.json.

```
    "repositories": {
        "drupal": {
            "type": "composer",
            "url": "https://packages.drupal.org/8"
        },
        "lullabot/message_integration": {
            "type": "vcs",
            "url": "https://github.com/lullabot/message_integration.git",
            "no-api": true
        }
    },

```

- Make sure all the required code is available by running
 `composer require lullabot/message_integration`.
- Enable this module, which will install all the required modules:
 `drush en message_implementation`
- Navigate to `admin/config/message/message` to adjust message settings.
- Navigate to `admin/structure/message` to edit and change message templates.
- Edit the display mode for each content type to position the subscription flags
 where you want them.
- Subscribe to some content.
- Create/edit content and add comments to it.
- Run cron to trigger queued messages and emails.
- Navigate to `admin/content/messages` to see the generated messages.
- Users will be subscribed to content and emails sent to them as content
 changes.

## To customize:

Review the code in `message_implementation.module` to see what hooks are being
 used to generate messages. You can alter them as needed.
