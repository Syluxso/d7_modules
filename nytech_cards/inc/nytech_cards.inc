<?php

module_load_include('inc', 'nytech_cards', 'nytech_cards.form');


function endpoint_cards() {
  return 'test';
}

function endpoint_deck_new() {
  $form_id = 'nytech_deck_edit_form';
  $form = drupal_get_form($form_id);
  return drupal_render($form);
}

function endpoint_decks() {
  $decks = new NyTechUsersDecks();
  $new_deck = l('+Deck', 'decks/create');
  $output = '';

  if(!empty($decks->decks)) {
    foreach($decks->decks as $deck) {
      $deck_class = new NyTechDeck($deck);
      $deck_class->set_output();
      $output .= $deck_class->output;
    }
  }
  return $new_deck . '<hr />' . $output;
}

function endpoint_deck_single($id) {
  $deck = entity_load_single('deck', $id);
  $deck_class = new NyTechDeck($deck);
  $manage_cards = l('Manage Cards', 'decks/' . $deck_class->deck_id . '/cards/list');
  $edit_deck = l('Edit Deck', 'decks/' . $id . '/edit');
  $run_shuffle_link = l('Shuffle', 'decks/' . $deck_class->deck_id . '/run/shuffle');
  $run_ordered_link = l('In Order', 'decks/' . $deck_class->deck_id . '/run/created');
  return $deck_class->title . ' ' . $edit_deck . '<hr />' . '<h3>' . $deck_class->card_count . ' cards ' . $manage_cards . '</h3> <h3>' . $run_shuffle_link . ' | ' . $run_ordered_link . '</h3>';
}

function endpoint_deck_edit($id) {
  $form_id = 'nytech_deck_edit_form';
  $form = drupal_get_form($form_id, $id);
  return drupal_render($form);
}

function endpoint_deck_run($id, $run_type = 'shuffle') {
  $deck = entity_load_single('deck', $id);
  $run = new NyTechRunDeck($deck, $run_type);
  return '<div class="cards--container">' . $run->output . '</div>';
}

function endpoint_deck_new_card($id) {
  $form_id = 'nytech_deck_card_edit_form';
  $form = drupal_get_form($form_id, $id);
  return drupal_render($form);
}

function endpoint_deck_cards_list($id) {
  $deck = entity_load_single('deck', $id);
  $deck_class = new NyTechDecksCards($deck);
  $deck_class->set_cards();
  $deck_class->set_output_mange_cards();
  $add_card = l('+Card', 'decks/' . $deck_class->deck_id . '/cards/create');

  return $add_card . '<h3>' . $deck_class->count . ' cards</h3>' . $deck_class->output;
}

function endpoint_deck_cards_edit($id, $card_id) {
  $form_id = 'nytech_deck_card_edit_form';
  $form = drupal_get_form($form_id, $id, $card_id);
  return drupal_render($form);
}

function endpoint_deck_cards_delete($id, $card_id) {
  entity_delete('card', $card_id);
  drupal_set_message('Card deleted.');
  drupal_goto('decks/' . $id . '/cards/list');
}

function endpoint_deck_delete($id) {
  $deck = entity_load_single('deck', $id);
  $d_class = new NyTechDecksCards($deck);
  $d_class->set_cards();
  entity_delete_multiple('card', $d_class->ids);
  entity_delete('deck', $id);
  drupal_set_message('Deck and its cards have been deleted.');
  drupal_goto('decks');
}

function endpoint_deck_import($id) {
  $deck = entity_load_single('deck', $id);
  $d = new NyTechImportDeck($deck);
  drupal_set_message($d->message);
  drupal_goto($d->destination);
}
