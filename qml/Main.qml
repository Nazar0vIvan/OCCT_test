import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Qt.labs.platform as Platform

import OCCT_test.Backend 1.0

ApplicationWindow {
  id: root

  visible: true
  width: 1000
  height: 700
  title: "OCCT minimal"

  Platform.MenuBar {
    window: root

    Platform.Menu {
      title: qsTr("File")

      Platform.MenuItem {
        text: qsTr("Exit")
        onTriggered: root.close()
      }
    }
  }

  WindowContainer {
    id: occContainer
    anchors.fill: parent
    window: OccController.viewWindow

    Component.onDestruction: {
      occContainer.window = null
    }
  }
}
