import maya.cmds as m
import maya.mel as mel

#List of locators from the Maya Scene file
arm = ['Rshoulderblade', 'Rshoulder', 'Relbow', 'Rfrontankle', 'Rfrontpaw', 'Rfrontclaws']
leg = ['Rthigh', 'Rknee', 'Rbackankle', 'Rbackpaw', 'Rbackclaws']
head = ['neck2', 'neck1','baseSkull','nose']
back =  ['back1', 'back2', 'lumbar1', 'lumbar2', 'lumbar3', 'lumbar4' ]
tail = ['tailbase', 'tail1', 'tail2', 'tail3','tail4', 'tail5', 'tail6','tail7']
iks = ['RankleJNT', 'LankleJNT', 'RbackankleJNT', 'LbackankleJNT']
handles = ['RarmIKHandle', 'LarmIKHandle', 'RlegIKHandle', 'LlegIKHandle']

###lock attributes
def lockTranslate(name):
	m.setAttr( name+'.tx', lock=True )	
	m.setAttr( name+'.ty', lock=True )		
	m.setAttr( name+'.tz', lock=True )	

def lockRotate(name):
	m.setAttr( name+'.rx', lock=True )	
	m.setAttr( name+'.ry', lock=True )		
	m.setAttr( name+'.rz', lock=True )	

def lockScale(name):
	m.setAttr( name+'.sx', lock=True )	
	m.setAttr( name+'.sy', lock=True )		
	m.setAttr( name+'.sz', lock=True )

###Freeze Transforms
def freeze(name):
    m.select(name)
    m.makeIdentity(apply=True, t=1, r=1, s=1, n=0)
	

#Build joint chains
def buildChain(list):
    
    for i in range(len(list)):        
        x = m.getAttr(list[i]+'.translateX')
        y = m.getAttr(list[i]+'.translateY')    
        z = m.getAttr(list[i]+'.translateZ')
	   
        m.joint(n = list[i]+'JNT', sc=True, p = [x,y,z], radius = .3 )	

joints = [arm,leg,head,tail,back]

for i in range(len(joints)):
    buildChain(joints[i])
    m.joint(joints[i][0]+'JNT', e = 1, zso = 1, oj = 'xyz', sao = 'yup', ch = 1)#orient joint chains     	 
    m.select(clear=True)  

#group bones
#m.group(joints[0][0]+'JNT',joints[1][0]+'JNT',joints[2][0]+'JNT',joints[3][0]+'JNT', n = 'boneGRP')
mel.eval("select -r RshoulderJNT.rotateAxis ;rotate -r -os 180 0 0 ;")
mel.eval("select -r RkneeJNT.rotateAxis ;rotate -r -os 180 0 0 ;")


#mirror arms n legs
m.mirrorJoint('RshoulderbladeJNT', mirrorXY=True, searchReplace=('R','L'), mb=True)
m.mirrorJoint('RthighJNT', mirrorXY=True, searchReplace=('R','L'))

#orient mirrored joints
m.joint('LshoulderbladeJNT', e = 1, zso = 1, oj = 'xyz', sao = 'yup', ch = 1)  
m.joint('LthighJNT', e = 1, zso = 1, oj = 'xyz', sao = 'yup', ch = 1) 
mel.eval("select -r LshoulderJNT.rotateAxis ;rotate -r -os 180 0 0 ;")
mel.eval("select -r LkneeJNT.rotateAxis ;rotate -r -os 180 0 0 ;")


#Create IK on arms n legs
m.ikHandle(n = 'RarmIKHandle', sj='RshoulderJNT', ee='RfrontankleJNT', sol = 'ikRPsolver', s = 'sticky')
m.ikHandle(n = 'LarmIKHandle', sj='LshoulderJNT', ee='LfrontankleJNT', sol = 'ikRPsolver', s = 'sticky')
m.ikHandle(n = 'RlegIKHandle', sj='RthighJNT', ee='RbackankleJNT', sol = 'ikRPsolver', s = 'sticky')
m.ikHandle(n = 'LlegIKHandle', sj='LthighJNT', ee='LbackankleJNT', sol = 'ikRPsolver', s = 'sticky')

############
#Create IKControl Rfrontankle
m.circle(n = 'RarmIKCON', c = (0,0,0), nr = (1,0,0), r = 1)
m.setAttr( 'RarmIKCON.ove', True)
m.setAttr( 'RarmIKCON.ovc', 13)

m.xform(cp=True)  #Center the Pivot
m.pointConstraint('RfrontankleJNT', 'RarmIKCON') #Move the group to the joint position
m.orientConstraint('RfrontankleJNT', 'RarmIKCON') #Orient the group to the joint
m.delete('RarmIKCON_orientConstraint1') #Delete the constraints so that the 
m.delete('RarmIKCON_pointConstraint1')  #control maintains position and orientation

#Connect the control to the IK handle
#freeze('RarmIKHandle')
m.pointConstraint('RarmIKCON', 'RarmIKHandle', mo=True)
freeze('RarmIKCON')
lockRotate('RarmIKCON')
lockScale('RarmIKCON')	


#Create IKControl Lfrontankle
m.circle(n = 'LarmIKCON', c = (0,0,0), nr = (1,0,0), r = 1)
m.setAttr( 'LarmIKCON.ove', True)
m.setAttr( 'LarmIKCON.ovc', 6)

m.xform(cp=True)  #Center the Pivot
m.pointConstraint('LfrontankleJNT', 'LarmIKCON') #Move the group to the joint position
m.orientConstraint('LfrontankleJNT', 'LarmIKCON') #Orient the group to the joint
m.delete('LarmIKCON_orientConstraint1') #Delete the constraints so that the 
m.delete('LarmIKCON_pointConstraint1')  #control maintains position and orientation

#Connect the control to the IK handle
#freeze('LarmIKHandle')
m.pointConstraint('LarmIKCON', 'LarmIKHandle', mo=True)
freeze('LarmIKCON')
lockRotate('LarmIKCON')
lockScale('LarmIKCON')


#Create IKControl Rbackankle
m.circle(n = 'RlegIKCON', c = (0,0,0), nr = (1,0,0), r = 1)
m.setAttr( 'RlegIKCON.ove', True)
m.setAttr( 'RlegIKCON.ovc', 13)

m.xform(cp=True)  #Center the Pivot
m.pointConstraint('RbackankleJNT', 'RlegIKCON') #Move the group to the joint position
m.orientConstraint('RbackankleJNT', 'RlegIKCON') #Orient the group to the joint
m.delete('RlegIKCON_orientConstraint1') #Delete the constraints so that the 
m.delete('RlegIKCON_pointConstraint1')  #control maintains position and orientation

#Connect the control to the IK handle
#freeze('RlegIKHandle')
m.pointConstraint('RlegIKCON', 'RlegIKHandle', mo=True)
freeze('RlegIKCON')
lockRotate('RlegIKCON')
lockScale('RlegIKCON')


#Create IKControl Lbackankle
m.circle(n = 'LlegIKCON', c = (0,0,0), nr = (1,0,0), r = 1)
m.setAttr( 'LlegIKCON.ove', True)
m.setAttr( 'LlegIKCON.ovc', 6)

m.xform(cp=True)  #Center the Pivot
m.pointConstraint('LbackankleJNT', 'LlegIKCON') #Move the group to the joint position
m.orientConstraint('LbackankleJNT', 'LlegIKCON') #Orient the group to the joint
m.delete('LlegIKCON_orientConstraint1') #Delete the constraints so that the 
m.delete('LlegIKCON_pointConstraint1')  #control maintains position and orientation


#Connect the control to the IK handle
#freeze('LlegIKHandle')
m.pointConstraint('LlegIKCON', 'LlegIKHandle', mo=True)
freeze('LlegIKCON')
lockRotate('LlegIKCON')
lockScale('LlegIKCON')
#########

####group controls
m.group('RarmIKCON', 'LarmIKCON', 'RlegIKCON', 'LlegIKCON', n='ControlGRP') 
###########
#m.group(n='ControlGRP')
m.group(n = 'boneGRP')
m.group('boneGRP', 'ControlGRP', n='GlobalScale')
m.group('GlobalScale', n='GlobalTranslate')


lockTranslate('GlobalScale')
lockRotate('GlobalScale')
lockScale('GlobalTranslate')

#m.group('RotateZ', n='RotateY')
#m.group('RotateY', n='RotateX')
#m.group('RotateX', n='GlobalTranslate')

m.group('RarmIKHandle', 'LarmIKHandle', 'RlegIKHandle', 'LlegIKHandle', n='IK')
#m.group(n='IK')
lockTranslate('IK')
lockRotate('IK')
lockScale('IK')
#group IK
m.group('CheetahGeo', 'IK', n='noXform')
freeze('CheetahGeo')
lockTranslate('CheetahGeo')
lockRotate('CheetahGeo')
lockScale('CheetahGeo')
lockTranslate('noXform')
lockRotate('noXform')
lockScale('noXform')

m.group('noXform', 'GlobalTranslate', n='Cheetah')
lockTranslate('Cheetah')
lockRotate('Cheetah')
lockScale('Cheetah')
lockTranslate('boneGRP')
lockRotate('boneGRP')
lockScale('boneGRP')
lockTranslate('ControlGRP')
lockRotate('ControlGRP')
lockScale('ControlGRP')
	


##Build back controls
def boxControls(list):

	start = 'createNode transform -n \"'
	mid = '\";\n\tsetAttr \".ove\" yes;\n\tsetAttr \".ovc\" 17;\n\tsetAttr \".s\" -type \"double3\" 0.38788995794875847 0.38788995794875847 	  0.38788995794875847 ;\n\tsetAttr \".rp\" -type \"double3\" 	  -0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\n\tsetAttr \".sp\" -type \"double3\" -0.10486440153690055 0.69282883991548905 		-0.99995762933988319 ;\ncreateNode nurbsCurve -n \"curveShape1\" -p \"'
	end = '\";\n\tsetAttr -k off \".v\";\n\tsetAttr \".cc\" -type \"nurbsCurve\" \n\t\t1 15 0 no 3\n\t\t16 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15\n\t\t16\n\t\t0.63056699999999999 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -1.6914370000000001\n\t\t;'

	for i in range(len(list)):
		curveName = list[i] + 'CON'		
    
     		mel.eval(start + curveName + mid + curveName + end)
		m.xform(cp=True)  #Center the Pivot
		m.pointConstraint(list[i] + 'JNT', curveName) #Move the CON to the joint position
		m.orientConstraint(list[i]+ 'JNT', curveName) #Orient the group to the joint
		m.delete(curveName + '_orientConstraint1') #Delete the constraints so that the 		
		m.delete(curveName + '_pointConstraint1')  
		#offset control
		x = m.getAttr(curveName+'.translateX')
        	y = m.getAttr(curveName+'.translateY')    
        	z = m.getAttr(curveName+'.translateZ')
		m.move(x,y+1.23376,z)

		freeze(curveName)
			
		m.orientConstraint(curveName, list[i] + 'JNT', mo=True)		
		

		lockTranslate(curveName)
		lockScale(curveName)					

boxControls(back)
#boxControls(head)

mel.eval("move 2.85618 8.094783 0 back1CON.scalePivot back1CON.rotatePivot ;")
mel.eval("move 1.49487 8.042768 0 back2CON.scalePivot back2CON.rotatePivot ;")
mel.eval("move -0.805392 8.462507 0 lumbar1CON.scalePivot lumbar1CON.rotatePivot ;")
mel.eval("move -1.816855 8.719012 0 lumbar2CON.scalePivot lumbar2CON.rotatePivot ;")
mel.eval("move -2.798979 8.895504 0 lumbar3CON.scalePivot lumbar3CON.rotatePivot ;")
mel.eval("move -3.9854 8.966226 0 lumbar4CON.scalePivot lumbar4CON.rotatePivot ;")


###Build head controls
def headControls(list):

	start = 'createNode transform -n \"'
	mid = '\";\n\tsetAttr \".ove\" yes;\n\tsetAttr \".ovc\" 17;\n\tsetAttr \".s\" -type \"double3\" 0.18788995794875847 0.18788995794875847 	  0.18788995794875847 ;\n\tsetAttr \".rp\" -type \"double3\" 	  -0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\n\tsetAttr \".sp\" -type \"double3\" -0.10486440153690055 0.69282883991548905 		-0.99995762933988319 ;\ncreateNode nurbsCurve -n \"curveShape1\" -p \"'
	end = '\";\n\tsetAttr -k off \".v\";\n\tsetAttr \".cc\" -type \"nurbsCurve\" \n\t\t1 15 0 no 3\n\t\t16 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15\n\t\t16\n\t\t0.63056699999999999 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -1.6914370000000001\n\t\t;'

	for i in range(len(list)-2):
		curveName = list[i] + 'CON'		
    
     		mel.eval(start + curveName + mid + curveName + end)
		m.xform(cp=True)  #Center the Pivot
		m.pointConstraint(list[i] + 'JNT', curveName) #Move the CON to the joint position
		m.orientConstraint(list[i]+ 'JNT', curveName) #Orient the group to the joint
		m.delete(curveName + '_orientConstraint1') #Delete the constraints so that the 		
		m.delete(curveName + '_pointConstraint1')  
		#offset control
		x = m.getAttr(curveName+'.translateX')
        	y = m.getAttr(curveName+'.translateY')    
        	z = m.getAttr(curveName+'.translateZ')
		
		m.move(x,y+2.23376,z)

		freeze(curveName)
			
		m.orientConstraint(curveName, list[i] + 'JNT', mo=True)		
		

		lockTranslate(curveName)
		lockScale(curveName)			

	m.circle(n = 'baseSkullCON', c = (0,0,0), nr = (1,0,0), r = 1.5)
	m.setAttr( 'baseSkullCON.ove', True)
	m.setAttr( 'baseSkullCON.ovc', 17)
	m.xform(cp=True)  #Center the Pivot
	m.pointConstraint('baseSkullJNT', 'baseSkullCON') #Move the group to the joint position
	#m.orientConstraint('baseSkullJNT', 'baseSkullCON') #Orient the group to the joint
	#m.delete('baseSkullCON_orientConstraint1') #Delete the constraints so that the 
	m.delete('baseSkullCON_pointConstraint1')  #control maintains position and orientation
	freeze('baseSkullCON')
	lockTranslate('baseSkullCON')
	lockScale('baseSkullCON')
	#Connect the control to the joint
	m.orientConstraint('baseSkullCON', 'baseSkullJNT', mo=True)

headControls(head)

mel.eval("move 4.691706 7.983795 0 neck2CON.scalePivot neck2CON.rotatePivot ;")
mel.eval("move 5.458543 8.455148 0 neck1CON.scalePivot neck1CON.rotatePivot ;")


##Build tail controls
def tailControls(list):

    for i in range(len(list)-1):
	m.circle(n = list[i]+'CON', c = (0,0,0), nr = (1,0,0), r = 1)
	m.setAttr( list[i]+'CON.ove', True)
	m.setAttr( list[i]+'CON.ovc', 17)
	m.xform(cp=True)  #Center the Pivot
	m.pointConstraint(list[i]+'JNT', list[i]+'CON') #Move the group to the joint position
	m.orientConstraint(list[i]+'JNT', list[i]+'CON') #Orient the group to the joint
	m.delete(list[i]+'CON_orientConstraint1') #Delete the constraints so that the 
	m.delete(list[i]+'CON_pointConstraint1')  #control maintains position and orientation
	freeze(list[i]+'CON')
	lockTranslate(list[i]+'CON')
	lockScale(list[i]+'CON')
	#Connect the control to the joint
	m.orientConstraint(list[i]+'CON', list[i]+'JNT', mo=True)	
	

tailControls(tail)

feet = ['Rfrontpaw','Lfrontpaw','Rbackpaw','Lbackpaw']


##Build foot controls
def footControls(list):

    for i in range(len(list)):
	m.circle(n = list[i]+'CON', c = (0,0,0), nr = (1,0,0), r = 1)
	m.setAttr( list[i]+'CON.ove', True)
	m.setAttr( list[i]+'CON.ovc', 17)
	if(i==0):
		m.setAttr( list[i]+'CON.ove', True)
		m.setAttr( list[i]+'CON.ovc', 13)
	if(i==1):
		m.setAttr( list[i]+'CON.ove', True)
		m.setAttr( list[i]+'CON.ovc', 6)
	if(i==2):
		m.setAttr( list[i]+'CON.ove', True)
		m.setAttr( list[i]+'CON.ovc', 13)
	if(i==3):
		m.setAttr( list[i]+'CON.ove', True)
		m.setAttr( list[i]+'CON.ovc', 6)
	m.xform(cp=True)  #Center the Pivot
	m.pointConstraint(list[i]+'JNT', list[i]+'CON') #Move the group to the joint position
	#m.orientConstraint(list[i]+'JNT', list[i]+'CON') #Orient the group to the joint
	#m.delete(list[i]+'CON_orientConstraint1') #Delete the constraints so that the 
	m.delete(list[i]+'CON_pointConstraint1')  #control maintains position and orientation

	x = m.getAttr(list[i]+'CON.translateX')
        y = m.getAttr(list[i]+'CON.translateY')    
        z = m.getAttr(list[i]+'CON.translateZ')
	m.rotate(0,0,90)	
	m.move(x,y-.53376,z)

	freeze(list[i]+'CON')
	lockTranslate(list[i]+'CON')
	lockScale(list[i]+'CON')
	#Connect the control to the joint
	m.orientConstraint(list[i]+'CON', list[i]+'JNT', mo=True)	   
	

footControls(feet)

mel.eval("move -4.794807 0.406479 1.071492 RbackpawCON.scalePivot RbackpawCON.rotatePivot ;")
mel.eval("move -4.794804 0.406479 -1.071493 LbackpawCON.scalePivot LbackpawCON.rotatePivot ;")
mel.eval("move 4.48876 0.414079 0.792634 RfrontpawCON.scalePivot RfrontpawCON.rotatePivot ;")
mel.eval("move 4.488726 0.414131 -0.790178 LfrontpawCON.scalePivot LfrontpawCON.rotatePivot ;")



poles = ['RelbowJNT', 'LelbowJNT','RkneeJNT' , 'LkneeJNT']


##Build pole vector controls
def poleVectorControls(list):

	start = 'createNode transform -n \"'
	mid = '\";\n\tsetAttr \".ove\" yes;\n\tsetAttr \".ovc\" 17;\n\tsetAttr \".s\" -type \"double3\" 0.38788995794875847 0.38788995794875847 	   		0.38788995794875847 ;\n\tsetAttr \".rp\" -type \"double3\"-0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\n\tsetAttr \".sp\" -type 		\"double3\" -0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\ncreateNode nurbsCurve -n \"curveShape1\" -p \"'
	end = '\";\n\tsetAttr -k off \".v\";\n\tsetAttr \".cc\" -type \"nurbsCurve\" \n\t\t1 15 0 no 3\n\t\t16 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15\n\t\t16\n\t\t0.63056699999999999 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 		-1.6914370000000001\n\t\t-0.82945800000000003 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 2.33924e-08 -1.6914370000000001\n\t\t0.63056699999999999 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 	-1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 2.33924e-08 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 		-0.23141200000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -0.23141200000000001\n\t\t-0.82945800000000003 2.33924e-08 -1.6914370000000001\n\t\t;'

	for i in range(len(list)):
		curveName = list[i]+'poleCON'		
    
     		mel.eval(start + curveName + mid + curveName + end)
		m.xform(cp=True)  #Center the Pivot
		m.pointConstraint(poles[i], curveName) #Move the CON to the joint position		
		m.delete(curveName + '_pointConstraint1')  
		x = m.getAttr(curveName+'.translateX')
        	y = m.getAttr(curveName+'.translateY')    
        	z = m.getAttr(curveName+'.translateZ')
		if(i==0):
			m.setAttr( curveName+'.ove', True)
			m.setAttr( curveName+'.ovc', 13)
		if(i==1):
			m.setAttr( curveName+'.ove', True)
			m.setAttr( curveName+'.ovc', 6)
		if(i==2):
			m.setAttr( curveName+'.ove', True)
			m.setAttr( curveName+'.ovc', 13)
		if(i==3):
			m.setAttr( curveName+'.ove', True)
			m.setAttr( curveName+'.ovc', 6)
		if(i<2):
			m.move(x-2,y,z)		
		if(i>1):
			m.move(x+2,y,z)		
		m.poleVectorConstraint(curveName, list[i])
		m.parent(curveName, 'ControlGRP')
		#offset control		
		freeze(curveName)
		lockRotate(curveName)
		lockScale(curveName)


poleVectorControls(handles)

mel.eval("joint -p 2.85618 8.094783 0 ;")
m.rename('joint1','chestPivotJNT')
#m.rename('joint2','pelvisPivotJNT')
m.setAttr('chestPivotJNT.radius', .3)
#m.setAttr('pelvisPivotJNT.radius', .3)	

			
###chestCON
mel.eval("createNode transform -n \"chestCON\";\n\tsetAttr \".ove\" yes;\n\tsetAttr \".ovc\" 17;\n\tsetAttr \".s\" -type \"double3\" 2.4455603746549071 1.86376209280223 1.7505636715552726 ;\n\tsetAttr \".rp\" -type \"double3\" -0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\n\tsetAttr \".sp\" -type \"double3\" -0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\ncreateNode nurbsCurve -n \"curveShape3\" -p \"chestCON\";\n\tsetAttr -k off \".v\";\n\tsetAttr \".cc\" -type \"nurbsCurve\" \n\t\t1 15 0 no 3\n\t\t16 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15\n\t\t16\n\t\t1.1749454962739798 2.339239999775225e-08 -2.0259774835265487\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-1.3738364962739791 2.339239999775225e-08 -2.0259774835265487\n\t\t1.1749454962739798 2.339239999775225e-08 -2.0259774835265487\n\t\t1.1749454962739798 2.339239999775225e-08 0.10312848352654827\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-1.3738364962739791 2.339239999775225e-08 0.10312848352654827\n\t\t1.1749454962739798 2.339239999775225e-08 0.10312848352654827\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-1.3738364962739791 2.339239999775225e-08 0.10312848352654827\n\t\t-1.3738364962739791 2.339239999775225e-08 -2.0259774835265487\n\t\t;")

m.xform(cp=True)  #Center the Pivot
m.pointConstraint('chestPivotJNT', 'chestCON') #Move the CON to the joint position		
m.delete('chestCON_pointConstraint1')

#offset control
x = m.getAttr('chestCON.translateX')
y = m.getAttr('chestCON.translateY')    
z = m.getAttr('chestCON.translateZ')
m.move(x,y-0.992458,z)
#m.pointConstraint('chestCON','chestPivotJNT', mo=True)
m.orientConstraint('chestCON', 'chestPivotJNT', mo=True)
m.parent('chestCON', 'ControlGRP')
freeze('chestCON')
lockScale('chestCON')

mel.eval("move 2.85618 8.094783 0 chestCON.scalePivot chestCON.rotatePivot ;")



'''###pelvisCON
mel.eval("createNode transform -n \"pelvisCON\";\n\tsetAttr \".ove\" yes;\n\tsetAttr \".ovc\" 17;\n\tsetAttr \".s\" -type \"double3\" 2.4455603746549071 1.86376209280223 1.7505636715552726 ;\n\tsetAttr \".rp\" -type \"double3\" -0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\n\tsetAttr \".sp\" -type \"double3\" -0.10486440153690055 0.69282883991548905 -0.99995762933988319 ;\ncreateNode nurbsCurve -n \"curveShape3\" -p \"pelvisCON\";\n\tsetAttr -k off \".v\";\n\tsetAttr \".cc\" -type \"nurbsCurve\" \n\t\t1 15 0 no 3\n\t\t16 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15\n\t\t16\n\t\t1.1749454962739798 2.339239999775225e-08 -2.0259774835265487\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-1.3738364962739791 2.339239999775225e-08 -2.0259774835265487\n\t\t1.1749454962739798 2.339239999775225e-08 -2.0259774835265487\n\t\t1.1749454962739798 2.339239999775225e-08 0.10312848352654827\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t0.63056699999999999 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -1.6914370000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-1.3738364962739791 2.339239999775225e-08 0.10312848352654827\n\t\t1.1749454962739798 2.339239999775225e-08 0.10312848352654827\n\t\t0.63056699999999999 1.460024 -0.23141200000000001\n\t\t-0.82945800000000003 1.460024 -0.23141200000000001\n\t\t-1.3738364962739791 2.339239999775225e-08 0.10312848352654827\n\t\t-1.3738364962739791 2.339239999775225e-08 -2.0259774835265487\n\t\t;")

m.xform(cp=True)  #Center the Pivot
m.pointConstraint('lumbar4JNT', 'pelvisCON') #Move the CON to the joint position		
m.delete('pelvisCON_pointConstraint1')

#offset control
x = m.getAttr('pelvisCON.translateX')
y = m.getAttr('pelvisCON.translateY')    
z = m.getAttr('pelvisCON.translateZ')
m.move(x,y-0.992458,z)
#m.pointConstraint('chestCON','chestPivotJNT', mo=True)
m.orientConstraint('pelvisCON', 'lumbar4JNT', mo=True)
m.parent('pelvisCON', 'ControlGRP')
freeze('pelvisCON')
lockScale('pelvisCON')'''


#Global Translate control
mel.eval("createNode transform -n \"GlobalCON\";\ncreateNode nurbsCurve -n \"curveShape1\" -p \"GlobalCON\";\n\tsetAttr -k off \".v\";\n\tsetAttr \".cc\" -type \"nurbsCurve\" \n\t\t1 24 0 no 3\n\t\t25 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24\n\t\t25\n\t\t0 0 -7.8805394146532812\n\t\t-3.1522157658613099 0 -4.7283236487919682\n\t\t-1.576107882930655 0 -4.7283236487919682\n\t\t-2.1731120232037231 0 -2.1731120232037231\n\t\t-4.7283236487919682 0 -1.576107882930655\n\t\t-4.7283236487919682 0 -3.1522157658613099\n\t\t-7.8805394146532812 0 0\n\t\t-4.7283236487919682 0 3.1522157658613099\n\t\t-4.7283236487919682 0 1.576107882930655\n\t\t-2.1731120232037231 0 2.1731120232037231\n\t\t-1.576107882930655 0 4.7283236487919682\n\t\t-3.1522157658613099 0 4.7283236487919682\n\t\t0 0 7.8805394146532812\n\t\t3.1522157658613099 0 4.7283236487919682\n\t\t1.576107882930655 0 4.7283236487919682\n\t\t2.1731120232037231 0 2.1731120232037231\n\t\t4.7283236487919682 0 1.576107882930655\n\t\t4.7283236487919682 0 3.1522157658613099\n\t\t7.8805394146532812 0 0\n\t\t4.7283236487919682 0 -3.1522157658613099\n\t\t4.7283236487919682 0 -1.576107882930655\n\t\t2.1731120232037231 0 -2.1731120232037231\n\t\t1.576107882930655 0 -4.7283236487919682\n\t\t3.1522157658613099 0 -4.7283236487919682\n\t\t0 0 -7.8805394146532812\n\t\t;")

m.xform(cp=True)  #Center the Pivot		
#m.parentConstraint('GlobalCON', 'back1JNT', mo=True)
#m.parentConstraint('GlobalCON', 'chestPivotJNT', mo=True)
m.parent('GlobalCON', 'Cheetah')


#connect parent joint chains
m.parent('RshoulderbladeJNT', 'chestPivotJNT')
m.parent('LshoulderbladeJNT', 'chestPivotJNT')
m.parent('neck2JNT', 'chestPivotJNT')
#m.parent('back1JNT', 'chestPivotJNT')

m.parent('RthighJNT', 'lumbar4JNT')
m.parent('LthighJNT', 'lumbar4JNT')
m.parent('tailbaseJNT', 'lumbar4JNT')
m.parent('chestPivotJNT', 'boneGRP')
m.parent('back1JNT', 'boneGRP')
m.parent('back1JNT','chestPivotJNT')

m.parent('back1CON', 'ControlGRP')
m.parent('back2CON', 'back1CON')
m.parent('lumbar1CON', 'back2CON')
m.parent('lumbar2CON', 'lumbar1CON')
m.parent('lumbar3CON', 'lumbar2CON')
m.parent('lumbar4CON', 'lumbar3CON')

m.parent('tailbaseCON', 'lumbar4CON')
m.parent('tail1CON', 'tailbaseCON')
m.parent('tail2CON', 'tail1CON')
m.parent('tail3CON', 'tail2CON')
m.parent('tail4CON', 'tail3CON')
m.parent('tail5CON', 'tail4CON')
m.parent('tail6CON', 'tail5CON')

m.parent('neck2CON', 'ControlGRP')
m.parent('neck1CON', 'neck2CON')
m.parent('baseSkullCON', 'neck1CON')

m.parent('RarmIKHandlepoleCON', 'RarmIKCON')
m.parent('LarmIKHandlepoleCON', 'LarmIKCON')
m.parent('RlegIKHandlepoleCON', 'RlegIKCON')
m.parent('LlegIKHandlepoleCON', 'LlegIKCON')

m.parent('RfrontpawCON', 'RarmIKCON')
m.parent('LfrontpawCON', 'LarmIKCON')
m.parent('RbackpawCON', 'RlegIKCON')
m.parent('LbackpawCON', 'LlegIKCON')


m.connectAttr('GlobalCON.translate','GlobalTranslate.translate')
m.connectAttr('GlobalCON.rotate','GlobalTranslate.rotate')
m.connectAttr('GlobalCON.scale','GlobalScale.scale')

#mel.eval("move -r 0 0 0 GlobalTranslate.scalePivot GlobalTranslate.rotatePivot ;")





