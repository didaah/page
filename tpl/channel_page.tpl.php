<?php
// $Id$

/**
 * 频道页面模板
 * @param object $channel 频道对象，$channel->type 为频道系统名称
 * 模板加载顺序：
 *  channel_page_{$channel->type}.tpl.php
 *  channel_page.tpl.php
 */
?>

<div class="channel_page channel_page_<?php echo $channel->type;?>">
  <?php echo $channel->content;?>
</div>
