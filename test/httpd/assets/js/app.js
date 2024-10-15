import $ from "jquery";
window.$ = $;
window.jQuery = $;

import "bootstrap";

import hljs from "highlight.js";
$(function () {
    hljs.highlightAll();
});

import "./fx";
import "./kmx_anim";
import "./pixel";

var test = document.getElementsByTagName('video');
for (var i = 0; i <= test.length; i++){
  var testa = function (i) {
    test[i++];
  }
  $(test[i]).attr("ended", testa);       
}
