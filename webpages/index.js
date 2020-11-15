var colorSelect
var redSelect
var greenSelect
var blueSelect
var applyButton
var color

window.onload = init

function init() {
  colorSelect = document.color_select.color
  redSelect = document.color_select.red
  greenSelect = document.color_select.green
  blueSelect = document.color_select.blue
  applyButton = document.getElementById('submit')

  colorSelect.addEventListener("change", onColorChange, false)
  redSelect.addEventListener("change", onColorChange, false)
  greenSelect.addEventListener("change", onColorChange, false)
  blueSelect.addEventListener("change", onColorChange, false)

  color = parseInt(colorSelect.value.substring(1, 7), 16)
  redSelect.value = color >> 16
  greenSelect.value = (color & 0x00FF00) >> 8
  blueSelect.value = color & 0x0000FF
  applyButton.style.backgroundColor = '#' + (color ^ 0xFFFFFF).toString(16).padStart(6, '0')
}

function onColorChange() {
  if (this.id == "color") {
    color = parseInt(this.value.substring(1, 7), 16)
  } else if (this.id == "red") {
    color = color & 0x00FFFF | this.value << 16
  } else if (this.id == "green") {
    color = color & 0xFF00FF | this.value << 8
  } else if (this.id == "blue") {
    color = color & 0xFFFF00 | this.value
  }

  var hexColor = '#' + color.toString(16).padStart(6, '0')
  document.body.style.backgroundColor = hexColor
  colorSelect.value = hexColor
  redSelect.value = color >> 16
  greenSelect.value = (color & 0x00FF00) >> 8
  blueSelect.value = color & 0x0000FF
  applyButton.style.color = hexColor
  applyButton.style.backgroundColor = '#' + (color ^ 0xFFFFFF).toString(16).padStart(6, '0')
}
