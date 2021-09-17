<?php

class NyTechUsersDecks {
  public $uid;
  public $ids;
  public $decks;

  function __construct($uid = null) {
    $this->set_uid($uid);
    $this->set_cards();
  }

  private function set_uid($uid = null) {
    if(empty($uid)) {
      global $user;
      $uid = $user->uid;
    }
    $this->uid = (int) $uid;
  }

  private function set_cards() {
    if(!empty($this->uid)) {
      $ids = [];
      $decks = [];
      $q = db_query('SELECT d.* FROM {deck} d WHERE d.uid = :uid AND d.state = :state', array(':uid' => $this->uid, ':state' => 1));
      foreach($q as $row) {
        $ids[] = $row->id;
        $decks[] = $row;
      }
      $this->ids = $ids;
      $this->decks = $decks;
    }
  }
}

class NyTechDeck {
  public $entity;
  public $deck_id;
  public $title;
  public $card_ids;
  public $cards;
  public $card_count;

  public $output;

  function __construct($entity) {
    $this->entity = $entity;
    $this->deck_id = $this->entity->id;
    $this->title = $this->entity->title;
    $this->set_cards();
  }

  public function set_cards() {
    $cards_class = new NyTechDecksCards($this->entity);
    $this->cards = $cards_class->cards;
    $this->card_ids = $cards_class->ids;
    $this->card_count = $cards_class->count;
  }

  public function set_output() {
    $output = '<h3>' . l($this->title, 'decks/' . $this->deck_id) . '</h3>';
    $this->output = $output;
  }
}


class NyTechDecksCards {
  public $deck_id;

  public $ids;
  public $cards;
  public $count;

  public $deck;

  public $output;

  function __construct($deck) {
    $this->deck = $deck;
    $this->deck_id = $this->deck->id;
    $this->set_cards();
  }

  public function set_cards() {
    $q = db_query('SELECT c.* FROM {card} c WHERE c.binder_id = :deck_id', array(':deck_id' => $this->deck_id));
    $ids = [];
    $cards = [];
    foreach($q as $row) {
      $ids[] = $row->id;
      $cards[] = $row;
    }
    $this->ids = $ids;
    $this->cards = $cards;
    $this->count = count($this->ids);
  }

  public function set_output_mange_cards() {
    $output = '';
    if(!empty($this->cards)) {
      foreach($this->cards as $card) {
        $output .= l($card->front . ' (' . $card->back . ')', 'decks/' . $this->deck_id . '/cards/' . $card->id . '/edit') . '<br />';
      }
    }
    $this->output = $output;
  }
}

class NyTechRunDeck {
  public $burndown;

  function __construct($deck, $run_type = 'shuffle') {
    $this->deck = $deck;
    $this->deck_id = $this->deck->id;
    $this->run_type = $run_type;
    $this->count = 0;
    $this->set_cards();
    $this->set_burndown();
    $this->burndown();
    $this->set_output();
  }

  private function set_cards() {
    $data = new NyTechDecksCards($this->deck);
    $cards = [];
    foreach($data->cards as $card) {
      $cards[$card->id] = $card;
    }
    $this->cards = $cards;
    $this->ids = $data->ids;
  }

  private function set_burndown() {
    $burndown = unserialize($this->deck->blob);
    $ids = $this->ids;
    $this->burndown = $ids;
    if(empty($burndown)) {
      switch ($this->run_type) {
        case 'shuffle':
          shuffle($this->burndown);
          break;
        case 'created':
          // This is the natural order.
          break;
        default:
          shuffle($this->burndown);
          break;
      }
      array_push($this->burndown, 'end');
      $this->deck->blob = serialize($this->burndown);
      entity_save('deck', $this->deck);
    } else {
      $this->burndown = $burndown;
    }
    $this->count = count($this->burndown);
  }

  private function burndown() {
    if(!empty($this->burndown)) {
      $burndown = $this->burndown;
      $burndown = array_values($burndown);
      $current_card_id = reset($burndown);

      if($current_card_id == 'end') {
        $this->deck->blob = null;
        entity_save('deck', $this->deck);
        drupal_goto('decks/' . $this->deck_id);
      } else {
        $this->current_card = $this->cards[$current_card_id]; // Set the current card from the random array.
        $this->front = $this->current_card->front;
        $this->back = $this->current_card->back;
        unset($burndown[0]);
        $this->burndown = $burndown;
        $this->deck->blob = serialize($this->burndown);
        entity_save('deck', $this->deck);
      }
    }
  }

  private function set_output() {
    $front = '';
    $back = '';
    if(!empty($this->front)) {
      $front = '<h2>' . $this->front . '</h2>';
    }
    if(!empty($this->back)) {
      $back = '<h4>' . $this->back . '</h4>';
    }
    $next_link = l('Next >', 'decks/' . $this->deck_id . '/run');
    $output = theme('flip_card', array('front' => $front, 'back' => $back, 'next' => $next_link, 'cards_left' => $this->count -1 ));
    $this->output = $output;
  }
}

class NyTechImportDeck {
  public $deck;
  public $cards;
  public $count;
  public $uid;
  public $created;
  public $changed;

  function __construct($deck) {
    global $user;
    $this->deck = $deck;
    $this->uid = $user->uid;
    $this->created = time();
    $this->changed = time();
    $this->import_deck();
    $this->import_cards();
  }

  private function import_deck() {
    if(!empty($this->deck)) {
      $entity = entity_create('deck', array(
        'type' => 'standard',
        'uid' => $this->uid,
        'title' => $this->deck->title,
        'state' => 1,
        'status' => 1,
        'created' => $this->created,
        'changed' => $this->changed,
      ));
      entity_save('deck', $entity);
      $this->entity = $entity;
    }
  }

  private function import_cards() {
    $c = new NyTechDecksCards($this->deck);
    $c->set_cards();
    if(!empty($c->cards)) {
      foreach($c->cards as $card) {
        $new_card = entity_create('card', array(
          'type' => 'standard',
          'uid' => $this->uid,
          'state' => 1,
          'status' => 1,
          'created' => $this->created,
          'changed' => $this->changed,
          'front' => $card->front,
          'back' => $card->back,
          'binder_type' => 'card',
          'binder_id' => $this->entity->id,
        ));
        entity_save('card', $new_card);
      }
    }
    $this->message = '"' . $this->entity->title . '" deck imported with ' . count($c->ids) . ' cards.';
    $this->destination = 'decks';
  }
}
