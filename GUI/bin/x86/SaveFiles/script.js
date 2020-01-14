var commentsHidden = false;
var classesHidden = false;
var functionHidden = false;

function toggleVisibility(classname) {
    var elements = document.getElementsByClassName(classname);
    if (classname == 'comments') {
        if (commentsHidden) commentsHidden = false;
        else commentsHidden = true;
        var i;
        for (i = 0; i < elements.length; i++) {
            elements[i].hidden = commentsHidden;
        }
    }
	
    if (classname == 'classes') {
        if (classesHidden) classesHidden = false;
        else classesHidden = true;
        var i;
        for (i = 0; i < elements.length; i++) {
            elements[i].hidden = classesHidden;
        }
    }
	
    if (classname == 'function') {
        if (functionHidden) functionHidden = false;
        else functionHidden = true;
        var i;
        for (i = 0; i < elements.length; i++) {
            elements[i].hidden = functionHidden;
        }
    }
}

