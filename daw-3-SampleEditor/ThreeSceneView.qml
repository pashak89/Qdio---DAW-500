import QtQuick 2.12
import QtWebEngine 1.10
import QtWebChannel 1.0

// Self-contained Three.js scene panel.
// Usage: ThreeSceneView { anchors.fill: parent }
//
// Requires "threeBridge" context property set in main.cpp.

Item {
    id: root

    WebChannel {
        id: channel
        registeredObjects: [threeBridge]
    }

    WebEngineView {
        id: webView
        anchors.fill: parent

        url: "qrc:/three_scene.html"
        webChannel: channel
        focus: true

        onLoadingChanged: {
            if (loadRequest.status === WebEngineView.LoadSucceededStatus)
                console.log("[ThreeSceneView] scene loaded")
            else if (loadRequest.status === WebEngineView.LoadFailedStatus)
                console.warn("[ThreeSceneView] load failed:", loadRequest.errorString)
        }

        onJavaScriptConsoleMessage: {
            console.log("[Three.js]", message, sourceID + ":" + lineNumber)
        }
    }
}
