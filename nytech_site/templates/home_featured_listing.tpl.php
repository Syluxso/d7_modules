<li>
  <article class="hentry has-featured">
    <div class="property-featured">
      <a class="content-thumb" href="<?php print base_path() . drupal_get_path_alias('node/' . $node_id); ?>">
        <img src="<?php print $listing_image_url; ?>" class="attachment-property-image" alt="" /> </a>
      <span class="property-category"><?php print $listing_type; ?></span>
    </div>
    <div class="property-wrap">
      <h2 class="property-title">
        <a href="<?php print base_path() . drupal_get_path_alias('node/' . $node_id); ?>" title="<?php print $node->field_listing_address['und'][0]['locality'] . ', ' . $node->field_listing_address['und'][0]['thoroughfare']; ?>">
          <?php print $node->field_listing_address['und'][0]['locality'] . ', ' . $node->field_listing_address['und'][0]['thoroughfare']; ?>
        </a>
      </h2>
      <div class="property-excerpt">
        <?php print substr($node->body['und'][0]['value'], 0, 144); ?>...
      </div>
      <div class="property-summary">
        <div class="property-detail">
          <div class="size"><span><?php print number($node->field_listing_sqft['und'][0]['value']); ?> sqft</span>
          </div>
          <div class="bathrooms"><span><?php print $node->field_listing_baths['und'][0]['value']; ?></span>
          </div>
          <div class="bedrooms"><span><?php print $node->field_listing_beds['und'][0]['value']; ?></span>
          </div>
        </div>
        <div class="property-info">
          <div class="property-price">
            <span><span class="amount"><?php print price($node->field_listing_price['und'][0]['value']); ?></span> </span>
          </div>
          <div class="property-action">
            <a href="<?php print base_path() . drupal_get_path_alias('node/' . $node_id); ?>">More Details <i class="fa fa-arrow-circle-o-right"></i></a>
          </div>
        </div>
      </div>
    </div>
  </article>
</li>
