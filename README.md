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
- [Diff](https://www.drupal.org/project/diff)


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
- Navigate to `admin/config/message/message` to adjust message settings if
 desired. The default settings are probably fine.
- Navigate to `admin/structure/message` to edit and change message templates.
- Navigate to `admin/structure/message-subscribe` to configure subscription
 settings. Be sure to check the option to queue messages so they are sent
 only on cron.
- Navigate to `admin/config/mailsystem` to adjust mail settings. Be sure
 Swiftmailer is set as the formatter and sender for the Message Notify module
 in mailsystem settings.
- Navigate to `admin/config/swiftmailer` and choose to use the 'HTML' format
 and uncheck `Respect provided e-mail format.`, which would revert everything
 to plain text.
- Edit the display mode for each content type to position the subscription flags
 where you want them on the content nodes.
- Subscribe to some content.
- Create/edit content and add comments to it.
- Run cron to trigger queued messages and emails.
- Navigate to `admin/content/messages` to see the generated messages.

The logic used for subscribing users to content and sending them emails
 includes the following. See `message_integration.module` and the message
 templates for more details.
- All active users will automatically be subscribed to all new content. They
 can later unsubscribe if they want.
- All authors will be subscribed to their existing content, and will be
 subscribed to new content along with other users.
- Newly published content will render the entire node in an email and send it
 to the subscriber list.
- New revisions will use Diff module to just display what changed in an email to
 the subscriber list (see the `node-diff` token in the
 `message_integration.tokens` file).
- New comments will notify the users subscribed to the node and just display
 the node title and author and the new comment in the email.
- Admin users will be notified by email when new users register.

## To customize:

Review the code in `message_implementation.module` to see what hooks are being
 used to generate messages. You can alter them as needed.