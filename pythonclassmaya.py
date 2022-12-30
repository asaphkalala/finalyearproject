#Python Class
import maya.cmds as cmds
class AK_Window(object):

    #constructor
    def __init__(self):

        self.window = "MR_Window"
        self.title = "Resize Tool"

        # close old window is open
        if cmds.window(self.window, exists = True):
            cmds.deleteUI(self.window, window=True)

            # create new window
            self.window = cmds.window(self.window, title=self.title, widthHeight=[10000,200] )

            # add a form layout to window
            self.mainForm = cmds.formLayout(numberOfDivisions=500)

            # add title to window
            self.titleDisplay = cmds.text(label = self.title, align="center",font="boldLabelFont")

            # position title on the rop left of the screen
            cmds.formLayout(self.mainForm, edit=True,attachForm=([self.titleDisplay, 'top', 5],[self.titleDisplay, 'left', 5],[self.titleDisplay, 'right', 5]))

            #seperator
            self.titleSeperator = cmds.separator();
            cmds.formLayout(self.mainForm, edit=True, attachControl=[self.titleSeperator, 'top', 10, self.titleDisplay], attachForm=([self.titleSeperator, 'left', 5], [self.titleSeperator, 'right', 5]) )

            #textfield for cube name
            self.cubeName = cmds.textFieldGrp(label="Object Name:")
            cmds.formLayout(self.mainForm, edit=True, attachControl=[self.cubeName, 'top', 10, self.titleSeperator], attachForm=[self.cubeName, 'left', 5])

            #group of three floats for specifying cube size
            self.cubeSize = cmds.floatFieldGrp( numberOfFields=3, label='Size:',value1=1, value2=1, value3=1)
            cmds.formLayout(self.mainForm, edit=True, attachControl=[self.cubeSize, 'top', 10, self.cubeName], attachForm=[self.cubeSize, 'left', 5] )

            #button for creating the cube
            self.cubeCreateBtn = cmds.button( label='Apply', width=50,command=self.createCube )

            cmds.formLayout(self.mainForm, edit=True, attachForm=[self.cubeCreateBtn, 'left', 50])

        #display new window
        cmds.showWindow()

        #function called by create button
        def createCube(self, *args):

            #If the same cube existed delete and receate new cube

            if cmds.polyCube(self.cubeName, exists = True):
                cmds.delete(self.cubeName, window=True)

                # get the value set by the user
                name = cmds.textFieldGrp(self.cubeName, query=True, text=True)
                width = cmds.floatFieldGrp(self.cubeSize, query=True, value1=True)
                height = cmds.floatFieldGrp(self.cubeSize, query=True, value2=True)
                depth = cmds.floatFieldGrp(self.cubeSize, query=True, value1=True)

                # create the cube using values set by user
                cmds.polyCube(name=name, width=width, height=height, depth=depth)

    myWindow = AK_Window()
