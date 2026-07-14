import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Layouts
import Qt.labs.platform as Platform

import OCCT_test.Backend 1.0

ApplicationWindow {
  id: root

  function readV6d(view, label) {
    const vals = []

    for (let idx = 0; idx < view.count; ++idx) {
      const item = view.model.get(idx)
      const text = String(item.value).trim()
      const val = Number(text)

      if (text.length === 0 || !isFinite(val)) {
        console.warn("Invalid", label, "at row", idx, ":", text)
        return []
      }

      vals.push(val)
    }

    return vals
  }

  function solveIK() {
    const pose = readV6d(lvIKInput, "IK input")

    if (pose.length !== 6) {
      return
    }

    OccController.solveIK(pose)
  }

  function solveFK() {
    const q = readV6d(lvFKInput, "FK input")

    if (q.length !== 6) {
      return
    }

    OccController.solveFK(q)
  }

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

  RowLayout {
    id: rlMain

    anchors.fill: parent
    spacing: 0

    Rectangle {
      id: occArea

      Layout.fillWidth: true
      Layout.fillHeight: true
      Layout.minimumWidth: 400
      Layout.minimumHeight: 300

      color: "#151515"
      border.width: 1
      border.color: "#444444"

      WindowContainer {
        id: occContainer

        anchors.fill: parent
        anchors.margins: 1

        focus: false
        activeFocusOnTab: false

        window: OccController.viewWindow

        Component.onDestruction: {
          occContainer.window = null
        }
      }
    }

    ColumnLayout {
      id: clIK

      property int lblWidth: 20

      Layout.preferredWidth: 260
      Layout.fillHeight: true
      Layout.alignment: Qt.AlignTop
      Layout.margins: 10
      spacing: 30

      ListView {
        id: lvIKInput

        Layout.preferredWidth: 260
        Layout.preferredHeight: 210
        spacing: 10

        interactive: false

        header: Label {
          text: "IK Input"
          font.bold: true
          bottomPadding: 10
        }

        model: ListModel {
          ListElement {name: "X:"; unit: "mm";  value: "0.00" }
          ListElement {name: "Y:"; unit: "mm";  value: "0.00" }
          ListElement {name: "Z:"; unit: "mm";  value: "0.00" }
          ListElement {name: "A:"; unit: "deg"; value: "0.00" }
          ListElement {name: "B:"; unit: "deg"; value: "0.00" }
          ListElement {name: "C:"; unit: "deg"; value: "0.00" }
        }

        delegate: RowLayout {

          width: lvIKInput.width

          Label {
            Layout.preferredWidth: 20
            Layout.preferredHeight: 24
            text: model.name
          }
          TextField {
            id: tfIk

            Layout.preferredWidth: 100
            Layout.preferredHeight: 24

            text: model.value
            selectByMouse: true
            activeFocusOnPress: true
            persistentSelection: false
            leftPadding: 5

            background: Rectangle {
              color: "transparent"
              border{width: 1; color: "black"}
            }

            onTextEdited: {
               lvIKInput.model.setProperty(index, "value", text)
             }

            onActiveFocusChanged: {
              if (activeFocus) {
                Qt.callLater(tfIk.selectAll)
              } else {
                tfIk.deselect()
              }
            }

          }
          Text {
            text: model.unit
          }
        }
      }

      Button {
        id: btnSolveIK

        padding: 5
        text: "Solve IK"

        onClicked: root.solveIK()

        background: Rectangle {
          color: "#e0e0e0"
          border{width: 1; color: "gray"}
          opacity: btnSolveIK.pressed ? 0.6 : btnSolveIK.hovered ? 1.0 : 0.6
        }
      }

      ListView {
        id: lvFKInput

        Layout.preferredWidth: 260
        Layout.preferredHeight: 210
        spacing: 10

        interactive: false

        header: Label {
          text: "FK Input"
          font.bold: true
          bottomPadding: 10
        }

        model: ListModel {
          ListElement {name: "q1:"; unit: "deg"; value: "0.00" }
          ListElement {name: "q2:"; unit: "deg"; value: "-90.00" }
          ListElement {name: "q3:"; unit: "deg"; value: "90.00" }
          ListElement {name: "q4:"; unit: "deg"; value: "0.00" }
          ListElement {name: "q5:"; unit: "deg"; value: "0.00" }
          ListElement {name: "q6:"; unit: "deg"; value: "0.00" }
        }

        delegate: RowLayout {

          width: lvFKInput.width

          Label {
            Layout.preferredWidth: 20
            Layout.preferredHeight: 24
            text: model.name
          }

          TextField {
            id: tfFk

            Layout.preferredWidth: 100
            Layout.preferredHeight: 24

            text: model.value
            selectByMouse: true
            activeFocusOnPress: true
            persistentSelection: false
            leftPadding: 5

            background: Rectangle {
              color: "transparent"
              border { width: 1; color: "black" }
            }

            onTextEdited: {
              lvFKInput.model.setProperty(index, "value", text)
            }

            onActiveFocusChanged: {
              if (activeFocus) {
                Qt.callLater(tfFk.selectAll)
              } else {
                tfFk.deselect()
              }
            }
          }

          Text {
            text: model.unit
          }
        }
      }

      Button {
        id: btnSolveFK

        padding: 5
        text: "Solve FK"

        onClicked: root.solveFK()

        background: Rectangle {
          color: "#e0e0e0"
          border{width: 1; color: "gray"}
          opacity: btnSolveFK.pressed ? 0.6 : btnSolveFK.hovered ? 1.0 : 0.6
        }
      }

      Item { Layout.fillHeight: true }
    }

    Item { Layout.fillWidth: true }
  }


}
