#scaling tool in python
import maya.cmds as cmds

#select the first object
selection = cmds.ls(selection = True)[0]

# returns x-min, y-min, z-min, x-max, y-max, z-max in that order.
boundingBox = cmds.xform(selection, query=True, boundingBox=True)

# get the width, height and depth of object form the bounding box
width = boundingBox[3] - boundingBox[0]
height = boundingBox[4] - boundingBox[1]
depth = boundingBox[5] - boundingBox[2]

# set a target size width, height and depth
targetWidth = 10
targetHeight = 6
targetDepth = 1

# determine the amount each dimensions needs to be scaled
scaleWidth = targetWidth / width
scaleHeight = targetHeight / height
scaleDepth = targetDepth / depth

# get existing scale values
objScale = cmds.xform(selection, query=True, scale=True)

# apply scale
cmds.xform(selection, scale=[scaleWidth * objScale[0], scaleHeight * objScale[1], scaleDepth * objScale[2]])