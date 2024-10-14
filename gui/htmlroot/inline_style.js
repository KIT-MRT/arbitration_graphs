function isElement(element) {
    return element instanceof Element || element instanceof HTMLDocument;
}

/**
 * get computed style for an element, excluding any default styles
 *
 * @param {DOM} element
 * @return {object} difference
 */
function getStylesWithoutDefaults(element) {

    // creating an empty dummy object to compare with
    var dummy = document.createElement('element-' + (new Date().getTime()));
    document.body.appendChild(dummy);

    // getting computed styles for both elements
    var defaultStyles = getComputedStyle(dummy);
    var elementStyles = getComputedStyle(element);

    // calculating the difference and ignoring webkit spam from chrome
    var diff = {};
    for (var key in elementStyles) {
        if (elementStyles.hasOwnProperty(key)
            && defaultStyles[key] !== elementStyles[key]
            && key.substring(0, 6) != "webkit") {

            diff[key] = elementStyles[key];
        }
    }

    // clear dom
    dummy.remove();

    return diff;
}


function rgba2hex(orig) {
    var a, isPercent,
        rgb = orig.replace(/\s/g, '').match(/^rgba?\((\d+),(\d+),(\d+),?([^,\s)]+)?/i),
        alpha = (rgb && rgb[4] || "").trim(),
        hex = rgb ?
            (rgb[1] | 1 << 8).toString(16).slice(1) +
            (rgb[2] | 1 << 8).toString(16).slice(1) +
            (rgb[3] | 1 << 8).toString(16).slice(1) : orig;

    if (alpha !== "") {
        a = alpha;
    } else {
        a = 1.;
    }
    return { 'hex': hex, 'alpha': a };
}

inlineStyle = function (element) {
    if (element instanceof SVGDefsElement) {
        return;
    }

    element.childNodes.forEach(child => {
        if (isElement(child)) {
            inlineStyle(child);
        }
    });

    if (element instanceof SVGSVGElement) {
        return
    }

    var styleCss = getStylesWithoutDefaults(element);

    styleText = ""
    for (var key in styleCss) {
        const keyCssText = key.replace(/[A-Z]/g, letter => `-${letter.toLowerCase()}`);
        styleText += keyCssText + ": ";

        // convert rgba double tupple to rgb hex with fill-opacity (for inkscape)
        if ((key == "fill" || key == "stroke") && styleCss[key] != "none") {
            rgba = rgba2hex(styleCss[key]);
            styleText += "#" + rgba['hex'] + "; " + key + "-opacity: " + rgba['alpha'] + "; ";

            // make sure box roundings work
        } else if (key == "rx" || key == "ry") {
            styleText += styleCss[key] + "; "
            element.setAttribute(key, styleCss[key]);

        } else {
            styleText += styleCss[key] + "; "
        }
    }

    if (styleText.length) {
        element.setAttribute('style', styleText);
    }

    element.removeAttribute("class");

    if (element.id != 'root-svg-group') {
        element.removeAttribute("id");
    }
}