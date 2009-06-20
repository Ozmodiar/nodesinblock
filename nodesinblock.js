// $Id$

function NodesInBlockCheckOptions() {
  var nibselect = $('#edit-nodesinblock-visibility-select');
  var selected = nibselect[0].options[nibselect[0].selectedIndex].value;
  if (selected != '') {
    toggleNodesInBlockTextarea('hide', selected);
  }
  else {
	toggleNodesInBlockTextarea('show', selected);
  }
}

/**
 * Show or hide visibility textarea.
 */
function toggleNodesInBlockTextarea(type, selected) {
  if (type == 'hide') {
    $('#nodesinblock_visibility_textarea').hide('slow');
    $('#edit-nodesinblock-visibility').val(selected);
  }
  else {
    $('#nodesinblock_visibility_textarea').show('slow'); 
  } 
}