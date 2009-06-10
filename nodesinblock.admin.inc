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
    '#description' => t('Select number of blocks to create and associate content types which nodes can be used to insert as content in a block. When you change the number, be sure to goto the <a href="@url">block configuration</a> page so the block settings are updated in the database. When at least one content type is selected, two fieldsets will become available after clicking the submit button where you can change general settings and associate content types per block.', array('@url' => url('admin/build/block'))),
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

    $form['general_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('General settings per block'),
    );

    for ($i = 0, $a = 1; $i < variable_get('nodesinblock_nrofblocks', 1); $i++, $a++) {
      $form['general_settings']['nodesinblock_friendlyname_'. $i] = array(
        '#type' => 'textfield',
        '#title' => t('User friendly name for block') .' '. $a,
        '#default_value' => check_plain(variable_get('nodesinblock_friendlyname_'. $i, t('Nodes in block '. $a))),
      	'#description' => t('Only use alphanumeric characters.'),
      );
      $form['general_settings']['nodesinblock_visibility_'. $i] = array(
        '#type' => 'select',
        '#title' => t('Visibility settings for block') .' '. $a,
        '#options' => array(
          t('Show on every page except the listed pages.'),
          t('Show on only the listed pages.')
        ),
        '#default_value' => variable_get('nodesinblock_visibility_'. $i, '1'),
      );
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
      '#description' => t('Define per content type which blocks are available. On the content node form, a select box will be available to select the appropriate block where the node content must be displayed. You can also choose the label of the fieldset and the "Collapsed fieldset" checkbox let\'s you choose if the fieldset on the node form is collapsed or not.'),
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
        );

        $form['contenttypes_block']['nodesinblock_'. $key .'_label'] = array(
          '#type' => 'textfield',
          '#title' => t('Fieldset label'),
          '#default_value' => check_plain(variable_get('nodesinblock_'. $key .'_label', t('Nodes in block'))),
          '#size' => 20,
        );
        $form['contenttypes_block']['nodesinblock_'. $key .'_collapsed'] = array(
          '#type' => 'checkbox',
          '#title' => t('Collapsed fieldset'),
          '#default_value' => variable_get('nodesinblock_'. $key .'_collapsed', TRUE),
          '#suffix' => '</div>',
        );
      }
    }
  }

  return system_settings_form($form);
}

/**
 * Menu callback to look at the queue for nodes in block.
 *
 * @param integer $block The delta of the block.
 * @param integer $links Whether to render the blocklinks or not.
 */
function nodesinblock_queue($form_state, $block = NULL, $links = TRUE) {
  $form = array();
  $form['nodes'] = array('#tree' => TRUE);

  $number_of_blocks = variable_get('nodesinblock_nrofblocks', 1);
  if ($block == NULL) $block = 0;

  // Blocklinks.
  $blocklinks = array();
  for ($i = 0, $a = 1; $i < $number_of_blocks; $i++, $a++) {
    $title = check_plain(variable_get('nodesinblock_friendlyname_'. $i, t('Nodes in block '. $a)));
    $delta = ($i != '0') ? '/'. $i : '';
    $blocklinks[] = array(
    	'title' => $title,
    	'href' => 'admin/settings/nodesinblock/queue'. $delta
    );
  }
  if ($links == TRUE) {
    $form['blocklinks'] = array('#type' => 'item', '#value' => theme('links', $blocklinks));
  }

  // Content create links.
  $createlinks = array();
  $saved_delta = $block + 1;
  $saved_types = variable_get('nodesinblock_contenttypes', array());
  foreach ($saved_types as $type) {
    $deltas = variable_get('nodesinblock_'. $type .'_block', array());
    if (isset($deltas[$saved_delta]) && $deltas[$saved_delta] != 0) {
      $createlinks[] = array(
      	'title' => t('Create new @type', array('@type' => $type)),
      	'href' => 'node/add/'. $type,
      );
    }
  }
  $form['createlinks'] = array('#type' => 'item', '#value' => theme('links', $createlinks));

  // Get nodes and store them in #nodes key.
  $query = "SELECT n.nid, n.title, n.status as node_status, nib.status as nodesinblock_status,
  					nib.weight, nib.render, nib.visibility FROM {nodes_in_block} nib INNER JOIN {node} n
  					on n.nid = nib.nid WHERE nib.delta = %d ORDER BY weight ASC";
  $nodes = db_query($query, $block);
  while ($row = db_fetch_object($nodes)) {
    $form['nodes'][$row->nid]['nid'] = array(
      '#type' => 'value',
      '#value' => $row->nid,
    );
    $form['nodes'][$row->nid]['title'] = array(
      '#type' => 'item',
      '#value' => $row->title,
    );
    $form['nodes'][$row->nid]['node_status'] = array(
      '#type' => 'item',
      '#value' => ($row->node_status == 1) ? t('published') : t('unpublished'),
    );
    $form['nodes'][$row->nid]['block_status'] = array(
      '#type' => 'checkbox',
      '#default_value' => $row->nodesinblock_status,
    );
    $form['nodes'][$row->nid]['node_render'] = array(
      '#type' => 'item',
      '#value' => _nodesinblock_render_status($row->render, TRUE),
    );
    $form['nodes'][$row->nid]['visibility'] = array(
      '#type' => 'item',
      '#value' => htmlentities($row->visibility),
    );
    $form['nodes'][$row->nid]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $row->weight,
      '#delta' => 30,
    );
  }

  // Block delta
  $form['delta'] = array('#type' => 'value', '#value' => $block);

  // Save button.
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  return $form;
}

/**
 * Submit callback, save weights after queue submit.
 */
function nodesinblock_queue_submit($form, &$form_state) {
  foreach ($form_state['values']['nodes'] as $node) {
    db_query("UPDATE {nodes_in_block} set weight = %d, status = %d WHERE delta = %d AND nid = %d", $node['weight'], $node['block_status'], $form_state['values']['delta'], $node['nid']);
  }
}

/**
 * Theme queue form.
 */
function theme_nodesinblock_queue($form) {
  $output = '';

  $rows = array();
  foreach (element_children($form['nodes']) as $nid) {
    $element = &$form['nodes'][$nid];

    $element['weight']['#attributes']['class'] = 'nodes-weight';

    $row = array();
    $row[] = drupal_render($element['block_status']);
    $row[] = drupal_render($element['title']);
    $row[] = drupal_render($element['node_status']);
    $row[] = drupal_render($element['node_render']);
    $row[] = drupal_render($element['visibility']);
    $row[] = drupal_render($element['weight']);
    $row[] = l(t('View'), 'node/'. $nid) .' - '. l(t('Edit'), 'node/'. $nid .'/edit', array('query' => drupal_get_destination()));

    $row = array_merge(array('data' => $row), array());
    $row['class'] = !empty($row['class']) ? $row['class'] .' draggable' : 'draggable';
    $rows[] = $row;
  }

  if ($rows) {

    drupal_add_tabledrag('nodesinblock', 'order', 'sibling', 'nodes-weight');

    $header = array(
      t('Block status'),
      t('Title'),
      t('Node status'),
      t('Render mode'),
      t('Visibility'),
      t('Weight'),
      t('Operations'),
    );
    if (isset($form['blocklinks']))
    $output .= drupal_render($form['blocklinks']);
    $output .= drupal_render($form['createlinks']);
    $output .= theme('table', $header, $rows, array('id' => 'nodesinblock'));
    $output .= drupal_render($form);
  }
  else {
    if (isset($form['blocklinks']))
    $output .= drupal_render($form['blocklinks']);
    $output .= drupal_render($form['createlinks']);
    $output .= t('No nodes found.');
  }

  return $output;
}

