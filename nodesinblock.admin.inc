<?php
// $Id$

/**
 * @file
 * Administration page for nodes in block.
 */

/**
 * Menu callback to configure general settings for nodes in block.
 */
function nodesinblock_settings() {
  $form = array();
  $content_types = node_get_types('names');
  $saved_types = variable_get('nodesinblock_contenttypes', array());

  $form['nodesinblock'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
    '#description' => t('Select number of blocks to create and associate content types which nodes can be used to insert as content in a block. When you change the number, be sure to goto the <a href="@url">block configuration</a> page so the block settings are updated in the database. When at least one content type is selected, two fieldsets will become available after clicking the submit button where you can change the names of the block and associate content types per block.', array('@url' => url('admin/build/block'))),
  );
  $form['nodesinblock']['nodesinblock_nrofblocks'] = array(
    '#type' => 'select',
    '#title' => t('Total number of blocks'),
    '#options' => drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)),
    '#default_value' => variable_get('nodesinblock_nrofblocks', 1),
  );
  $form['nodesinblock']['nodesinblock_contenttypes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types'),
    '#options' =>  $content_types,
    '#default_value' => $saved_types,
  );

  // Associate content type with block(s) and generate nice names for the blocks.
  if (!empty($saved_types)) {

    $form['userfriendly_names'] = array(
      '#type' => 'fieldset',
      '#title' => t('User friendly names for the blocks'),
      '#description' => t('Only use alphanumeric characters.'),
    );

    $a = 1;
    for ($i = 0; $i < variable_get('nodesinblock_nrofblocks', 1); $i++) {
      $form['userfriendly_names']['nodesinblock_friendlyname_'. $a] = array(
        '#type' => 'textfield',
        '#title' => t('Number ') . $a,
        '#default_value' => check_plain(variable_get('nodesinblock_friendlyname_'. $a, t('Nodes in block '. $a))),
      );
      $a++;
    }

    $blocks = module_invoke('nodesinblock', 'block', 'list');
    $block_options = array();
    $a = 1;
    foreach ($blocks as $key => $value) {
      $block_options[$a] = $value['info'];
      $a++;
    }

    $form['contenttypes_block'] = array(
      '#type' => 'fieldset',
      '#title' => t('Settings per content type'),
      '#description' => t('Define per content type which blocks are available. On the content node form, a select box will be available to select the appropriate block where the node content must be displayed.'),
    );

    // Iterate over selected types
    foreach ($saved_types as $key => $value) {
      if ($value != '0') {
        $form['contenttypes_block']['nodesinblock_'. $key .'_block'] = array(
          '#type' => 'checkboxes',
          '#title' => t('Blocks for @key', array('@key' => $key)),
          '#options' => $block_options,
          '#default_value' => variable_get('nodesinblock_'. $key .'_block', array()),
          '#prefix' => '<div style="float: left; margin-right: 10px;">',
          '#suffix' => '</div>',
        );
      }
    }
  }

  return system_settings_form($form);
}

/**
 * Menu callback to look at the queue for nodes in block.
 */
function nodesinblock_queue($form_state, $block = NULL) {
  $form = array();
  return $form;
}
