<?php
// $Id$

/**
 * @file
 * Administration page for nodes in block.
 */

function nodesinblock_settings() {
  $form = array();
  $content_types = node_get_types('names');
  $saved_types = variable_get('nodesinblock_contenttypes', array());

  $count = count($content_types);
  $total = array();
  for ($i = 0, $a = 1; $i < $count; $i++, $a++) {
    $total[$a] = $a;
  }

  $form['nodesinblock'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );
  $form['nodesinblock']['nodesinblock_nrofblocks'] = array(
    '#type' => 'select',
    '#title' => t('Number of blocks'),
    '#description' => t('Select number of blocks to create. Warning, data is lost if you diminish the number.'),
    '#options' => $total,
    '#default_value' => variable_get('nodesinblock_nrofblocks', 1),
  );
  $form['nodesinblock']['nodesinblock_contenttypes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types'),
    '#description' => t('Select content types which nodes can be used as content in a block.'),
    '#options' =>  $content_types,
    '#default_value' => $saved_types,
  );

  // Associate content type with block.
  if (!empty($saved_types)) {

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
    );

    // Iterate over selected types
    foreach ($saved_types as $key => $value) {
      if ($value != '0') {
        $form['contenttypes_block']['nodesinblock_'. $key .'_block'] = array(
          '#type' => 'select',
          '#title' => t('Block for @key', array('@key' => $key)),
          '#options' => $block_options,
          '#default_value' => variable_get('nodesinblock_'. $key .'_block', array()),
        );
      }
    }
  }

  return system_settings_form($form);
}
