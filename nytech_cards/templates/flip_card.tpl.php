<?php print $cards_left; ?> cards left

<div class="scene scene--card">
  <div class="card--flippable">
    <div class="card__face card__face--front">
      <?php print $front; ?>
    </div>
    <div class="card__face card__face--back">
      <?php print $back; ?>
    </div>
  </div>
</div>
<?php print $next; ?>

<script>
  var card = document.querySelector('.card--flippable');
  card.addEventListener( 'click', function() {
    card.classList.toggle('is-flipped');
  });
</script>
