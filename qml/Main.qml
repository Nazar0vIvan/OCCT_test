import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Dialogs
import QtQuick.Layouts
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
        Layout.fillHeight: true
        spacing: 10

        interactive: false

        header: Label {
          text: "IK Input"
          font.bold: true
          bottomPadding: 10
        }

        model: ListModel {
          ListElement {name: "X:"; unit: "mm" }
          ListElement {name: "Y:"; unit: "mm" }
          ListElement {name: "Z:"; unit: "mm" }
          ListElement {name: "A:"; unit: "deg" }
          ListElement {name: "B:"; unit: "deg" }
          ListElement {name: "C:"; unit: "deg" }
        }

        delegate: RowLayout {

          width: lvIKInput.width

          Label {
            Layout.preferredWidth: 20
            Layout.preferredHeight: 24
            text: model.name
          }
          TextField {
            id: control

            Layout.preferredWidth: 100
            Layout.preferredHeight: 24

            text: "0.00"
            selectByMouse: true
            leftPadding: 5

            background: Rectangle {
              color: "transparent"
              border{width: 1; color: "black"}
            }

            onActiveFocusChanged: {
              if (activeFocus) {
                selectAll()
              }
            }

          }
          Text {
            text: model.unit
          }
        }
      }

      ListView {
        id: lvIKOutput

        Layout.preferredWidth: 260
        Layout.fillHeight: true
        spacing: 10

        interactive: false

        header: Label {
          text: "IK Output"
          font.bold: true
          bottomPadding: 10
        }

        model: ListModel {
          ListElement {name: "q1:"; unit: "deg" }
          ListElement {name: "q2:"; unit: "deg" }
          ListElement {name: "q3:"; unit: "deg" }
          ListElement {name: "q4:"; unit: "deg" }
          ListElement {name: "q5:"; unit: "deg" }
          ListElement {name: "q6:"; unit: "deg" }
        }

        delegate: RowLayout {

          width: lvIKOutput.width

          Label {
            Layout.preferredWidth: 20
            Layout.preferredHeight: 24
            text: model.name
          }
          Label {
            Layout.preferredWidth: 100
            Layout.preferredHeight: 24

            text: "0.00"
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

        background: Rectangle {
          color: "#e0e0e0"
          border{width: 1; color: "gray"}
          opacity: btnSolveIK.pressed ? 0.6 : btnSolveIK.hovered ? 1.0 : 0.6
        }
      }

      Item { Layout.fillHeight: true }
    }

    Item { Layout.fillWidth: true }
  }


}
