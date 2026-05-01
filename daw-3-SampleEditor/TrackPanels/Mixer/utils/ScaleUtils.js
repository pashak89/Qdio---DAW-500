.pragma library

// TrackPanels/Mixer/utils/Global.js
var refSize = 1.0
var scale = 1.0

function radiusCal(value) {
    value = value * scale * refSize
    return Math.round(value)
}

function scaleSize2(value) {
    value = Math.round(value * scale * refSize / 2)
    return value
}

// === math: invert your DB(Y) curve ===
// DB = -42.3529 * Y^2 - 33.6471 * Y + 6
function decibel2yNorm(db) {
    var a = -42.3529
    var b = -33.6471
    var c = 6 - db

    var discriminant = b * b - 4 * a * c
    if (discriminant < 0)
        return NaN

    var sqrtD = Math.sqrt(discriminant)
    return (-b - sqrtD) / (2 * a)
}

function dbDisplayToYPixel(dbDisplay, h) {
    var realDb = -dbDisplay
    var yNorm = decibel2yNorm(realDb)
    return yNorm * h
}
