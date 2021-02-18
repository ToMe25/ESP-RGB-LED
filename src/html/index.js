var selection
var colorSelect
var redSelect
var greenSelect
var blueSelect
var fadeSelect
var timeSelect
var timeDiv
var applyButton
var applyDiv
var applyPlaceholder
var selection2
var colorSelect2
var redSelect2
var greenSelect2
var blueSelect2
var fadeSelect2
var timeSelect2
var applyButton2
var applyLabel2
var color
var fade
var time
var color2
var fade2
var time2
var interval

window.onload = init

interval = window.setInterval(checkColor, 5000)

function init() {
	selection = document.getElementById('selection')
	colorSelect = document.color_select.color
	redSelect = document.color_select.red
	greenSelect = document.color_select.green
	blueSelect = document.color_select.blue
	fadeSelect = document.color_select.fade
	timeSelect = document.color_select.time
	timeDiv = document.getElementById('timer')
	applyButton = document.getElementById('submit')
	applyDiv = document.getElementById('apply')
	applyPlaceholder = document.getElementById('applyplaceholder')

	selection2 = document.getElementById('selection2')
	colorSelect2 = document.color_select.color2
	redSelect2 = document.color_select.red2
	greenSelect2 = document.color_select.green2
	blueSelect2 = document.color_select.blue2
	fadeSelect2 = document.color_select.fade2
	timeSelect2 = document.color_select.time2
	applyButton2 = document.getElementById('submit2')
	applyLabel2 = document.getElementById('submit2label')

	colorSelect.addEventListener('change', onColorChange, false)
	redSelect.addEventListener('change', onColorChange, false)
	greenSelect.addEventListener('change', onColorChange, false)
	blueSelect.addEventListener('change', onColorChange, false)
	fadeSelect.addEventListener('change', onChange, false)
	timeSelect.addEventListener('change', onChange, false)
	document.getElementById('dual_color_toggle').addEventListener('click', toggleDualColor, false)

	colorSelect2.addEventListener('change', onColor2Change, false)
	redSelect2.addEventListener('change', onColor2Change, false)
	greenSelect2.addEventListener('change', onColor2Change, false)
	blueSelect2.addEventListener('change', onColor2Change, false)
	fadeSelect2.addEventListener('change', onChange, false)
	timeSelect2.addEventListener('change', onChange, false)

	color = parseInt(colorSelect.value.substring(1, 7), 16)
	fade = fadeSelect.value
	color2 = parseInt(colorSelect2.value.substring(1, 7), 16)
	fade2 = fadeSelect2.value

	setColor()

	if (!selection2.hidden) {
		selection.style.width = 'calc(50% - 4px)'
		selection2.style.width = 'calc(50% - 4px)'
		timeDiv.hidden = false
		applyDiv.hidden = true
		applyPlaceholder.hidden = false
	}
}

function onChange() {
	window.clearInterval(interval)
}

function onColorChange() {
	onChange();
	if (this.id == 'color') {
		color = parseInt(this.value.substring(1, 7), 16)
	} else if (this.id == 'red') {
		color = color & 0x00FFFF | this.value << 16
	} else if (this.id == 'green') {
		color = color & 0xFF00FF | this.value << 8
	} else if (this.id == 'blue') {
		color = color & 0xFFFF00 | this.value
	}
	setColor()
}

function onColor2Change() {
	onChange();
	if (this.id == 'color2') {
		color2 = parseInt(this.value.substring(1, 7), 16)
	} else if (this.id == 'red2') {
		color2 = color2 & 0x00FFFF | this.value << 16
	} else if (this.id == 'green2') {
		color2 = color2 & 0xFF00FF | this.value << 8
	} else if (this.id == 'blue2') {
		color2 = color2 & 0xFFFF00 | this.value
	}
	setColor()
}

function setColor() {
	var hexColor = '#' + color.toString(16).padStart(6, '0')
	selection.style.backgroundColor = hexColor
	colorSelect.value = hexColor
	redSelect.value = color >> 16
	greenSelect.value = (color & 0x00FF00) >> 8
	blueSelect.value = color & 0x0000FF
	applyButton.style.color = hexColor
	applyButton.style.backgroundColor = '#' + (color ^ 0xFFFFFF).toString(16).padStart(6, '0')

	var hexColor2 = '#' + color2.toString(16).padStart(6, '0')
	selection2.style.backgroundColor = hexColor2
	colorSelect2.value = hexColor2
	redSelect2.value = color2 >> 16
	greenSelect2.value = (color2 & 0x00FF00) >> 8
	blueSelect2.value = color2 & 0x0000FF
	applyButton2.style.color = hexColor2
	var invertedColor2 = '#' + (color2 ^ 0xFFFFFF).toString(16).padStart(6, '0')
	applyButton2.style.backgroundColor = invertedColor2
	applyLabel2.style.color = invertedColor2
}

function toggleDualColor(change = true) {
	if (change) {
		onChange();
	}
	if (selection2.hidden) {
		selection.style.width = 'calc(50% - 4px)'
		selection2.style.width = 'calc(50% - 4px)'
		selection2.hidden = false
		timeDiv.hidden = false
		applyDiv.hidden = true
		applyPlaceholder.hidden = false
	} else {
		selection2.hidden = true
		selection.style.width = 'calc(100% - 4px)'
		timeDiv.hidden = true
		applyDiv.hidden = false
		applyPlaceholder.hidden = true
	}
}

function checkColor() {
	fetch('properties.json', { method: 'get' })
		.then((res) => {
			return res.json()
		})
		.then((out) => {
			color = out.color
			if (fade != out.fade) {
				fadeSelect.value = fade = out.fade
			}
			if (time != out.time) {
				timeSelect.value = time = out.time
			}
			if (out.dual) {
				color2 = out.color2
				if (fade2 != out.fade2) {
					fadeSelect2.value = fade2 = out.fade2
				}
				if (time2 != out.time2) {
					timeSelect2.value = time2 = out.time2
				}
				if (selection2.hidden) {
					toggleDualColor(false)
				}
			} else if (!selection2.hidden) {
				toggleDualColor(false)
			}
			setColor()
		})
		.catch((err) => {
			throw err
		})
}
