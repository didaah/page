<?php
// $Id$

/**
 * @file 页面浏览默认模板
 * @param object $page
 */
?>

<div id="page_static_<?php echo $page->pid;?>" class="page_static_type page_static_type_<?php echo $page->type?>">
  <h2 class="page_static_title"><?php echo $page->languages[$page->language]->title?></h2>
  <div class="page_static_body"><?php echo $page->languages[$page->language]->body?></div>
</div>

