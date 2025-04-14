import $ from "jquery";

var onScroll = function () {
  var page = $("body.doc div.page");
  var offset = page.position();
  var bottom = $(window).height() + pageYOffset - page.outerHeight();
  if (bottom < 0)
    bottom = 0;
  $(".left-col").css("bottom", bottom + "px");
};

$(function () {
  $(document).on("scroll", onScroll);
  onScroll();
});
