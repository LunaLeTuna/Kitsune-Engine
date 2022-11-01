Win_title("Kitsune-Builder"); //names title

//basic create objects
var cube_model = new Model("./cube.obj");
var albido_create = new Texture("./create/container.png");
var specular_create = new Texture("./create/container_specular.png");

//player camera
var p_cam = new Camera();
p_cam.position = new Vector3(0,2,0);
p_cam.rotation = new Vector3(0,0,0);
SetMainCam(p_cam);

//player light
var p_torch = new PointLight();


var ellie = new Prop();
ellie.position = new Vector3(0,1,0);
ellie.scale = new Vector3(.5,.5,.5)
// ellie.model = cube_model;
// ellie.texture = albido_create;
// ellie.specular = specular_create;
var el = new PointLight();
el.position = new Vector3(2,1.5,2);

//
//player movement
//
 function pivot_point(place, center, rot){
    var resultx = Math.cos(rot)*(place.x-center.x)-Math.sin(rot)*(place.y-center.y)+center.x
    var resulty = Math.sin(rot)*(place.x-center.x)+Math.cos(rot)*(place.y-center.y)+center.y
    return {"x": resultx, "y": resulty};
 } 

  var w = false;
  var s = false;
  var a = false;
  var d = false;
  var shift = false;
  var space = false;
  
function buttonpushedpog(event){
    var keyCode = event;

    if (keyCode == 87) 
        w=true;
    
    if (keyCode == 83) 
        s=true;

    if (keyCode == 65)
        a=true;

    if (keyCode == 68)
        d=true;

    if (keyCode == 340)
        shift=true;

    if (keyCode == 32)
        space=true;

    //print(keyCode);
}

function buttonthedpog(event){
    var keyCode = event;

    if (keyCode == 87) 
        w=false;

    if (keyCode == 83) 
        s=false;

    if (keyCode == 65)
        a=false;

    if (keyCode == 68)
        d=false;

    if (keyCode == 340)
        shift=false;

    if (keyCode == 32)
        space=false;

}

Input.addEventListener("keypress", buttonpushedpog);
Input.addEventListener("keyrelease", buttonthedpog);

var m1 = false; //left
var m2 = false; //right
var m3 = false; //middle

function mousepushedpog(event){
  var keyCode = event;

    if (keyCode == 0) 
        m1=true;

    if (keyCode == 1) 
        m2=true;

    if (keyCode == 2) 
        m3=true;

//   print(keyCode);
}

function mousethedpog(event){
  var keyCode = event;

    if (keyCode == 0) 
        m1=false;

    if (keyCode == 1) 
        m2=false;

    if (keyCode == 2) 
        m3=false;

}

Input.addEventListener("mousepress", mousepushedpog);
Input.addEventListener("mouserelease", mousethedpog);

function radians(dgr){return dgr*(Math.PI/180);}
function degrees(rag){return rag*180/Math.PI;}

var firstMouse = true;
var yaw   = -90.0;
var pitch =  0.0;
var lastX =  800.0 / 2.0;
var lastY =  600.0 / 2.0;
var cameraFront = new Vector3(0,1,0);

var xpos;
var ypos;

function mousty(event){
    if(!m3) return;

    //print("mouse x: " + event.xpos);
    //print("mouse y: " + event.ypos);
    xpos = event.xpos
    ypos = event.ypos

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    let xoffset = xpos - lastX;
    let yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    let sensitivity = 0.1; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0)
        pitch = 89.0;
    if (pitch < -89.0)
        pitch = -89.0;

    let front = new Vector3();
    front.x = Math.cos(radians(yaw)) * Math.cos(radians(pitch));
    front.y = Math.sin(radians(pitch));
    front.z = Math.sin(radians(yaw)) * Math.cos(radians(pitch));
    cameraFront = front.Normalize();
    p_cam.LookAt(new Vector3(p_cam.position.x + cameraFront.x, p_cam.position.y + cameraFront.y, p_cam.position.z + cameraFront.z));
}

Input.addEventListener("mousemove", mousty);


function loadBrk(map) {

    let FILE = fs.readFileSync(map, "UTF-8");
    let LINES = FILE.split("\n");
    let totalLines = 0;
    let bricks = [];
    let cam = [];
    let environment = {};
    let currentBrick = -1;
    let scriptWarning = false;

    for (let line of LINES) {
        totalLines++;
        line = line.trim();
        switch (totalLines) {
            case 1: {
                if (line !== "B R I C K  W O R K S H O P  V0.2.0.0") {
                    console.error("ERROR: This set was created using an incompatible version of Brick Hill.");
                    return process.exit(0);
                }
                continue;
            }
            case 3: {
                const glColor = line.split(" ");
                const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
                environment["ambient"] = rgbToHex(RGB[2], RGB[1], RGB[0]);
                continue;
            }
            case 4: {
                const glColor = line.split(" ");
                const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
                environment["baseColor"] = rgbToHex(RGB[0], RGB[1], RGB[2]);
                continue;
            }
            case 5: {
                const glColor = line.split(" ");
                const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
                environment["skyColor"] = rgbToHex(RGB[0], RGB[1], RGB[2]);
                continue;
            }
            case 6: {
                environment["baseSize"] = Number(line);
                continue;
            }
            case 7: {
                environment["sunIntensity"] = Number(line);
                continue;
            }
            case 8: {
                environment["weather"] = Number(line);
                continue;
            }
        }

        const DATA = line.split(" ");
        const ATTRIBUTE = DATA[0].replace("+", "");
        const VALUE = DATA.slice(1).join(" ");
        switch (ATTRIBUTE) {
            case "NAME": {
                bricks[currentBrick].name = VALUE;
                continue;
            }
            case "ROT": {
                bricks[currentBrick].rotation = VALUE;
                continue;
            }
            case "SHAPE": {
                bricks[currentBrick].shape = VALUE;
                continue;
            }
            case "MODEL": {
                bricks[currentBrick].model = VALUE;
                continue;
            }
            case "NOCOLLISION": {
                bricks[currentBrick].collision = false;
                continue;
            }
            case "COLOR": {
                const colors = VALUE.split(" ");
                const color = convertRGB(colors[0], colors[1], colors[2]);
                let team = new Team(teams[teams.length - 1], rgbToHex(color[0], color[1], color[2]));
                teams[teams.length - 1] = team;
                continue;
            }
            case "LIGHT": {
                const colors = VALUE.split(' ');
                const lightRange = colors[3];
                const RGB = convertRGB(colors[0], colors[1], colors[2]);
                bricks[currentBrick].lightEnabled = true;
                bricks[currentBrick]._lightRange = lightRange;
                bricks[currentBrick]._lightColor = rgbToHex(RGB[0], RGB[1], RGB[2]);
                continue;
            }
            case "SCRIPT": {
                if (scriptWarning)
                    continue;
                scriptWarning = true;
                console.warn("WARNING: This set contains scripts. Scripts are incompatible with node-hill so they will not be loaded.");
                continue;
            }
            case "CAMERA_POS": {
                cam.pos = VALUE.split(" ");
                continue;
            }
            case "CAMERA_ROT": {
                cam.rot = VALUE.split(" ");
                continue;
            }
        }
        if (DATA.length === 10) {
          const RGB = convertRGB(DATA[6], DATA[7], DATA[8])

            let brick = {
              "xPos": Number(DATA[0]),
              "yPos": Number(DATA[1]),
              "zPos": Number(DATA[2]),
              "xScale": Number(DATA[3]),
              "yScale": Number(DATA[4]),
              "zScale": Number(DATA[5]),
              "color": rgbToHex(RGB[0], RGB[1], RGB[2]),
              "alpha": Number(DATA[9])
            }

            bricks.push(brick);
            currentBrick++;
        }
        // if (DATA[0] && DATA[0] === ">TEAM")
        //     teams.push(VALUE);
    }
    let World = {"Environment":environment, "Bricks":bricks, "Camera":cam}
    return World;
}


CursorPin(false);

var speed = 0.2;
var maxspeed = 1;


function rgbToHex(r, g, b) {
    return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
}
function rgbToBgr(rgb) {
    return rgb.substring(4, 6) + rgb.substring(2, 4) + rgb.substring(0, 2);
}
function rgbToDec(r, g, b) {
    const rgb = r | (g << 8) | (b << 16);
    return rgb.toString(10);
}
function hexToDec(hex, bgr = false) {
    hex = hex.replace(/^#/, "");
    const rgb = hexToRGB(hex);
    if (!bgr) {
        return rgbToDec(rgb[0], rgb[1], rgb[2]);
    }
    else {
        return rgbToDec(rgb[2], rgb[1], rgb[0]);
    }
}
function hexToRGB(hex) {
    hex = hex.replace(/^#/, "");
    const bigint = parseInt(hex, 16);
    const r = (bigint >> 16) & 255;
    const g = (bigint >> 8) & 255;
    const b = bigint & 255;
    return [r, g, b];
}
function convertRGB(r, g, b) {
    r = Number(r);
    g = Number(g);
    b = Number(b);
    r *= 255, g *= 255, b *= 255;
    return [Math.ceil(r), Math.ceil(g), Math.ceil(b)];
}
const COLOR_REGEX = /(\[#[0-9a-fA-F]{6}\])/g;
function formatHex(input) {
    const match = input.match(COLOR_REGEX);
    if (!match)
        return input;
    match.forEach((colorCode) => {
        let hexCol = colorCode.replace(/[\[#\]]/g, "").toUpperCase();
        hexCol = rgbToBgr(hexCol);
        input = input.replace(colorCode, `<color:${hexCol}>`);
    });
    return input;
}


function loadBrk(map) {

    let FILE = fs.readFileSync(map, "UTF-8");
    let LINES = FILE.split("\n");
    let totalLines = 0;
    let bricks = [];
    let cam = [];
    let environment = {};
    let currentBrick = -1;
    let scriptWarning = false;

    for (let line of LINES) {
        totalLines++;
        line = line.trim();
        switch (totalLines) {
            case 1: {
                if (line !== "B R I C K  W O R K S H O P  V0.2.0.0") {
                    print("ERROR: This set was created using an incompatible version of Brick Hill.");
                    return process.exit(0);
                }
                continue;
            }
            case 3: {
                const glColor = line.split(" ");
                const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
                environment["ambient"] = rgbToHex(RGB[2], RGB[1], RGB[0]);
                continue;
            }
            case 4: {
                const glColor = line.split(" ");
                const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
                environment["baseColor"] = rgbToHex(RGB[0], RGB[1], RGB[2]);
                continue;
            }
            case 5: {
                const glColor = line.split(" ");
                const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
                environment["skyColor"] = rgbToHex(RGB[0], RGB[1], RGB[2]);
                continue;
            }
            case 6: {
                environment["baseSize"] = Number(line);
                continue;
            }
            case 7: {
                environment["sunIntensity"] = Number(line);
                continue;
            }
            case 8: {
                environment["weather"] = Number(line);
                continue;
            }
        }

        const DATA = line.split(" ");
        const ATTRIBUTE = DATA[0].replace("+", "");
        const VALUE = DATA.slice(1).join(" ");
        switch (ATTRIBUTE) {
            case "NAME": {
                bricks[currentBrick].name = VALUE;
                continue;
            }
            case "ROT": {
                bricks[currentBrick].rotation = VALUE;
                continue;
            }
            case "SHAPE": {
                bricks[currentBrick].shape = VALUE;
                continue;
            }
            case "MODEL": {
                bricks[currentBrick].model = VALUE;
                continue;
            }
            case "NOCOLLISION": {
                bricks[currentBrick].collision = false;
                continue;
            }
            case "COLOR": {
                const colors = VALUE.split(" ");
                const color = convertRGB(colors[0], colors[1], colors[2]);
                let team = new Team(teams[teams.length - 1], rgbToHex(color[0], color[1], color[2]));
                teams[teams.length - 1] = team;
                continue;
            }
            case "LIGHT": {
                const colors = VALUE.split(' ');
                const lightRange = colors[3];
                const RGB = convertRGB(colors[0], colors[1], colors[2]);
                bricks[currentBrick].lightEnabled = true;
                bricks[currentBrick]._lightRange = lightRange;
                bricks[currentBrick]._lightColor = rgbToHex(RGB[0], RGB[1], RGB[2]);
                continue;
            }
            case "SCRIPT": {
                if (scriptWarning)
                    continue;
                scriptWarning = true;
                console.warn("WARNING: This set contains scripts. Scripts are incompatible with node-hill so they will not be loaded.");
                continue;
            }
            case "CAMERA_POS": {
                cam.pos = VALUE.split(" ");
                continue;
            }
            case "CAMERA_ROT": {
                cam.rot = VALUE.split(" ");
                continue;
            }
        }
        if (DATA.length === 10) {
          const RGB = convertRGB(DATA[6], DATA[7], DATA[8])

            let brick = {
              "xPos": Number(DATA[0]),
              "yPos": Number(DATA[1]),
              "zPos": Number(DATA[2]),
              "xScale": Number(DATA[3]),
              "yScale": Number(DATA[4]),
              "zScale": Number(DATA[5]),
              "color": rgbToHex(RGB[0], RGB[1], RGB[2]),
              "alpha": Number(DATA[9])
            }

            bricks.push(brick);
            currentBrick++;
        }
        // if (DATA[0] && DATA[0] === ">TEAM")
        //     teams.push(VALUE);
    }
    let World = {"Environment":environment, "Bricks":bricks, "Camera":cam}
    return World;
}

var monark = false;

var sxsss = 0;

var deltaTime = 0;
function tick(delta){

    deltaTime = deltaTime+0.01;

    let cam_rot = p_cam.rotation.y;

    let current = {x:0,y:0};

    let temp = {x:0,y:0};

    var cameraSpeed = 0.1;

    if(m3){
        if (w) {
            p_cam.position = new Vector3(cameraSpeed * cameraFront.x + p_cam.position.x,-cameraSpeed * cameraFront.y + p_cam.position.y,cameraSpeed * cameraFront.z + p_cam.position.z);
        } 
        if (s) {
            p_cam.position = new Vector3(-cameraSpeed * cameraFront.x + p_cam.position.x,cameraSpeed * cameraFront.y + p_cam.position.y,-cameraSpeed * cameraFront.z + p_cam.position.z);
        }
        if (a) {
            p_cam.position = new Vector3(cameraSpeed * cameraFront.z + p_cam.position.x,p_cam.position.y,-cameraSpeed * cameraFront.x + p_cam.position.z);
        } 
        if (d) {
            p_cam.position = new Vector3(-cameraSpeed * cameraFront.z + p_cam.position.x,p_cam.position.y,cameraSpeed * cameraFront.x + p_cam.position.z);
        } 
        if (shift) {
            p_cam.position = new Vector3(p_cam.position.x,p_cam.position.y-cameraSpeed,p_cam.position.z);
        } 
        if (space) {
            p_cam.position = new Vector3(p_cam.position.x,cameraSpeed + p_cam.position.y,p_cam.position.z);
        }

        // p_torch.position = p_cam.position;

        CursorPin(true);
    }else{
        firstMouse = true;
        CursorPin(false);
    }
        

    if(player.LinearVelocity.z < maxspeed)  player.LinearVelocity.z = maxspeed;
    if(player.LinearVelocity.z > -maxspeed)  player.LinearVelocity.z = -maxspeed;
    if(player.LinearVelocity.x < maxspeed) player.LinearVelocity.x = maxspeed;
    if(player.LinearVelocity.x > -maxspeed) player.LinearVelocity.x = -maxspeed;

    if(((w||s)||(a||d))){
        var new_place = pivot_point(temp, current, cam_rot);
        player.AddForce(new Vector3(new_place.x,0,new_place.y));
    }

}