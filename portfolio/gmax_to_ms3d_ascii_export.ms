-- Gmax ms3d ascii Export script by Michael Hester

--Readme:
--Selected object will be converted to editable mesh, then details displayed
--in the maxscript listener
--To use exporter, open up maxscript listener.  Select an object in the gmax scene then
--"run script" from the file menu, choose this script.  The listener will be filled with
--the text taht the ascii file would contain
--To "export the script, highlight from //copy end to //copy start and [ctrl]+[c] to copy
--then paste into a new text file, name it and save it.  You can then import into milkshape
--using import -> Milkshape 3d ASCII from Milkshape's file menu.


--WARNING: for large files it may take several copy/paste runs. 
--it is best to make sure you dont copy part of a line
(

obj = getCurrentSelection()
mymesh = obj[1]
convertToMesh mymesh

fn gettexturecoord vertexindex = (
	
	objj = getCurrentSelection()
	
	mymeshh = objj[1]
	
	if mymeshh.numtverts == 0 then	return  #(0.0, 0.0, 0.0)

--for vertexindex = 1 to mymeshh.verts.count do
--(	
	myvertex = mymeshh.verts[vertexindex]
	myvertexindex = vertexindex
	
	myvertex.pos
	
	myfaces = meshop.getFacesUsingVert mymeshh myvertexindex
	
	--print myfaces
	
	fn getbitarray array =
	(
		for i = 1 to array.count do
		(
			if array[i] then
			return i
		)
		return 0
	)
	
	myfaceindex = getbitarray myfaces
	
	
	mf = getface mymeshh myfaceindex
	
	tv = getTVface mymeshh myfaceindex
	
	--print myfaceindex
	--print tv
	--print mf
	
	
	fn matchindex facedata texturedata index =
	(
		for i = 1 to 3 do
		(
			if facedata[i] == index then return texturedata[i]
		)
		return 0
	)
	
	mytv = matchindex mf tv myvertexindex
	
	--if mytv == 0 then return #(0.0, 0.0, 0.0)
	myUVW = getTvert mymeshh mytv
	
	--print myUVW
	return myUVW
)







format "//copy start\n"

format "Frames: 30
Frame: 1

Meshes: 1\n"; 
havematerial = -1
if mymaterial == undefined 
then havematerial = -1
else havematerial = 0


format "\"% \" 0 %\n" mymesh.name (havematerial as integer)
print mymesh.verts.count 
for i = 1 to mymesh.verts.count do
(
	myUVW = gettexturecoord i
	
	format "0 % % % % % -1\n" mymesh.verts[i].pos[1] mymesh.verts[i].pos[2] mymesh.verts[i].pos[3] myUVW[1] (1.0-myUVW[2])
)
print mymesh.faces.count
--getFaceRNormals mymesh 1 
--to extract the vertex normals, match the normal to the index from getface

--v = #()
--newnormals = #()
--v[1]

for i=1 to mymesh.faces.count do
(
    normals = meshop.getFaceRNormals mymesh i
	format "% % %\n" normals[1][1] normals[1][2] normals[1][3]
	) 

--v
--v.count
--for i = 1 to v.count do
--(
--	format "% % %\n" v[i][1] v[i][2] v[i][3]
--)	
print mymesh.faces.count
for i = 1 to mymesh.faces.count do
(
	f = getFace mymesh i
	 normals = meshop.getFaceRNormals mymesh i
	format "0 % % % % % % 1\n" (f[1] as integer - 1) (f[2] as integer- 1) (f[3] as integer - 1) (f[1] as integer - 1) (f[2] as integer - 1) (f[3] as integer - 1)
)
format "\n"
mymaterial = mymesh.material
--mymaterial.maps
--mymaterial.ambient.r
if mymaterial == undefined 
then format "Materials: 0\n"
else
(
format "Materials: 1
\"%\"
% % % % \n" mymaterial.name mymaterial.ambient.r mymaterial.ambient.g mymaterial.ambient.b (mymaterial.opacity /100)
format "% % % %\n" mymaterial.diffuse.r mymaterial.diffuse.g mymaterial.diffuse.b (mymaterial.opacity /100)
format "% % % %\n" mymaterial.specular.r mymaterial.specular.g mymaterial.specular.b (mymaterial.opacity/100)
format "% % % %\n" mymaterial.selfillumcolor.r mymaterial.selfillumcolor.g mymaterial.selfillumcolor.b (mymaterial.selfillumamount/100)
format "% \n" (mymaterial.glossiness / 100) 
format "% \n" (mymaterial.opacity / 100)
	if mymaterial.maps[2] == undefined then format "\"\"\n"
	else
	(
format "\"%\"\n" mymaterial.maps[2].filename
	)
	if mymaterial.maps[1] == undefined then format "\"\"\n\n"
	else
	(
format "\"%\"\n\n" mymaterial.maps[1].filename
	)

)

format "Bones: 0
GroupComments: 0
MaterialComments: 0
BoneComments: 0
ModelComment: 0\n"

format "//copyend\n"
)
