import $ from 'jquery';

$(function () {
  $('.pixel').click(function (event) {
    const el = event.target;
    if (el.classList.contains('pixel-0')) {
      el.classList.remove('pixel-0');
      el.classList.add('pixel-1');
    }
    else if (el.classList.contains('pixel-1')) {
      el.classList.remove('pixel-1');
      el.classList.add('pixel-0');
    }
  });
});
