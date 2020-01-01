# Custom Message Implementation

Custom code for Message stack and Swiftmailer to create messages about
 new/changed nodes and comments and email them to subscribers.

 The following modules are used:

- [Message](https://www.drupal.org/project/message)
- [Message Notify](https://www.drupal.org/project/message_notify)
- [Message Subscribe](https://www.drupal.org/project/message_subscribe)
- [Message UI](https://www.drupal.org/project/message_ui)
- [Mailsystem](https://www.drupal.org/project/mailsystem)
- [Swiftmailer](https://www.drupal.org/project/swiftmailer)
- [Diff](https://www.drupal.org/project/diff)

## Using this code
The logic used for subscribing users to content and sending them emails
 includes the following. See `message_integration.module` and the message
 templates for more details.

- All active users will automatically be subscribed to all new content. They
 can later unsubscribe if they want.
- When a node is first published, the code will render the `message` view mode
 of the node in an HTML email and send it to the subscriber list.
- When a new revision is created in a published node, the code will use Diff
 module to just display what changed and send the diff in an HTML email to the
 subscriber list.
- When new comments are added, the code will notify the users subscribed to the
 node. The email will display the `compact` view mode of the content and the
 `default` view mode of the new comment an HTML email.
- Admin users will be notified by email when new users register, using a custom
 mailing list instead of the usual subscription model.

Test emails in a local development environment using the enclosed Lando file.
 Install and start [Lando](https://docs.lando.dev/) as usual, then visit
 `http://dr.lndo.site/` to create, subscribe to, and edit content, and
  `http://dr.mail.lndo.site/` to see the emails that are sent.

## To install on a new site:

This code is too opinionated to use as a normal contributed module. But you
 you can fork it, adjust it to your own needs, and use it elsewhere.

To use this on your own site, fork this code, then add the forked repository
 to your site's composer.json.

```
    "repositories": {
        "drupal": {
            "type": "composer",
            "url": "https://packages.drupal.org/8"
        },
        "forked-repo/message_integration": {
            "type": "vcs",
            "url": "https://github.com/forked-repo/message_integration.git",
            "no-api": true
        }
    },

```

- Make sure all the required code (with patches) is available by running
 `composer require forked-repo/message_integration`.
- Enable this module, which will install all the other required modules:
 `drush en message_implementation`
- Navigate to `admin/config/message/message-subscribe` to configure subscription
 settings. Be sure to check the option to queue messages so they are sent
 only on cron.
- Navigate to `admin/config/mailsystem` to adjust mail settings. Be sure
 Swiftmailer is set as the formatter and sender for the Message Notify module
 in mailsystem settings.
- Navigate to `admin/config/swiftmailer` and choose to use the `HTML` format
 and uncheck `Respect provided e-mail format`, which would revert everything
 to plain text if checked.
- Navigate to `admin/structure/message` to edit and change message templates.
- Edit the display mode for each content type to position the subscription flag
 where you want it on the `default` view mode of your content nodes, then
 enable the `message` view mode to control the display that will be rendered
 in messages.
- Subscribe to some content.
- Create/edit content and add comments to it.
- Run cron to trigger queued messages and emails.
- Navigate to `admin/content/messages` to see the generated messages.

Review the code in `message_implementation.module` to see what hooks are being
 used to generate messages. You can alter them as needed. You'll also want to
 edit the list of libraries used to generate HTML email to add your own theme
 libraries.
