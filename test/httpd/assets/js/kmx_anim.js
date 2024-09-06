import $ from "jquery";
import kmx_colors from "./kmx_colors";

function getRandomInt(max) {
  return Math.floor(Math.random() * max);
}

$(function() {
  setInterval(function() {
    const i = getRandomInt(kmx_colors.index.length);
    const name = kmx_colors.index[i];
    const color = kmx_colors[name];
    const x = (getRandomInt(99) - 48) * 0.002;
    $("a").css("text-shadow", "0 0 0");
    $("a:hover").css("text-shadow", x + "em 0 0 " + color);
  }, 200);
});
