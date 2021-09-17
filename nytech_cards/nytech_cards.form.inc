<?php

module_load_include('inc', 'nytech_cards', 'nytech_cards.classes');

function nytech_deck_edit_form($form, &$form_state, $id = null) {
  $deck = entity_load_single('deck', $id);
  $title = '';
  if(!empty($deck->title)) {
    $title = $deck->title;
  }
  if(!empty($id)) {
    $form['deck_id'] = array(
      '#type' => 'hidden',
      '#value' => $id,
    );
  }

  $form['title'] = array(
    '#title' => 'Title',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $title,
    '#attributes' => array(
      'placeholder' => 'Title of the deck',
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  $form['cancel'] = array(
    '#markup' => l('Delete', 'decks/' . $id . '/delete'),
  );

  $form['#validate'][] = 'nytech_deck_edit_form_validate';
  $form['#submit'][]   = 'nytech_deck_edit_form_submit';

  return $form;
}

function nytech_deck_edit_form_validate($form, &$form_state) {
  $values = $form_state['values'];
}

function nytech_deck_edit_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];
  if(!empty($values['deck_id'])) {
    $deck = entity_load_single('deck', $values['deck_id']);
    $deck->title = $values['title'];
    $deck->changed = time();
    $message = 'Deck updated';
  } else {
    $deck = entity_create('deck', array(
      'title' => $values['title'],
      'uid' => $user->uid,
      'created' => time(),
      'changed' => time(),
      'type' => 'standard',
      'state' => 1,
    ));
    $message = 'Deck created';
  }
  entity_save('deck', $deck);
  drupal_set_message($message);
  drupal_goto('decks/' . $deck->id);
}

function nytech_deck_card_edit_form($form, &$form_state, $deck_id, $card_id = null) {
  $card = entity_load_single('card', $card_id);
  $front = '';
  $back = '';
  if(!empty($card->front)) {
    $front = $card->front;
  }
  if(!empty($card->back)) {
    $back = $card->back;
  }
  $form['front'] = array(
    '#title' => 'Front',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $front,
    '#attributes' => array(
      'placeholder' => 'What is the capital of California?',
    ),
  );

  $form['back'] = array(
    '#title' => 'Back',
    '#type' => 'textfield',
    '#required' => true,
    '#default_value' => $back,
    '#attributes' => array(
      'placeholder' => '"Sacramento" is the capital of California.',
    ),
  );

  $form['deck_id'] = array(
    '#type' => 'hidden',
    '#required' => true,
    '#value' => $deck_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  $form['cancel'] = array(
    '#markup' => l('Delete', 'decks/' . $deck_id . '/cards/' . $card_id . '/delete'),
  );

  if(!empty($card_id)) {
    $form['card_id'] = array(
      '#type' => 'hidden',
      '#required' => true,
      '#value' => $card_id,
    );
  }

  $form['#validate'][] = 'nytech_deck_card_edit_validate';
  $form['#submit'][]   = 'nytech_deck_card_edit_submit';

  return $form;
}

function nytech_deck_card_edit_validate($form, &$form_state) {
  $values = $form_state['values'];
}

function nytech_deck_card_edit_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];

  if(!empty($values['card_id'])) {
    $card = entity_load_single('card', $values['card_id']);
    $card->front = $values['front'];
    $card->back = $values['back'];
    $card->changed = time();
    $destination = 'decks/' . $values['deck_id'] . '/cards/list';
  } else {
    $card = entity_create('card', array(
      'front' => $values['front'],
      'back' => $values['back'],
      'binder_id' => $values['deck_id'],
      'binder_type' => 'standard',
      'uid' => $user->uid,
      'created' => time(),
      'changed' => time(),
      'type' => 'standard',
      'state' => 1,
    ));
    $destination = 'decks/' . $values['deck_id'] . '/cards/create';
  }
  entity_save('deck', $card);
  drupal_set_message('Card updated.');
  drupal_goto($destination);
}
