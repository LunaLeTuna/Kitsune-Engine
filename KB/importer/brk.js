const { rgbToHex, convertRGB } = require('./KB/color/color.js');

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
                environment["baseColor"] = new Vector3(RGB[0]/255, RGB[1]/255, RGB[2]/255);
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
              "color": new Vector3(RGB[0]/255, RGB[1]/255, RGB[2]/255),
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

function BV(map, cube_model, plane_model, studs, rainbow, scene, dev_text) {

    var plane = new Prop();
    plane.scale = new Vector3(map.Environment.baseSize*0.5,0,map.Environment.baseSize*0.5);
    plane.model = plane_model;
    plane.texture = studs;
    plane.shader = rainbow;
    plane.setVec3("brick_size", new Vector3(map.Environment.baseSize,map.Environment.baseSize,map.Environment.baseSize));
    plane.setVec3("color", map.Environment.baseColor);
    plane.setBool("dont", true);
    plane.setBool("base", true);

    var b = map.Bricks.length;
    for (let i = 0; i < b; i++) {
        let objectc = new Prop();
        objectc.model = cube_model;
        objectc.texture = studs;

        // objectc.position = new Vector3(map.Bricks[i].yPos+map.Bricks[i].yScale/2, map.Bricks[i].zPos+map.Bricks[i].zScale/2, map.Bricks[i].xPos+map.Bricks[i].xScale/2);
        // objectc.scale = new Vector3(map.Bricks[i].yScale/2, map.Bricks[i].zScale/2, map.Bricks[i].xScale/2)

        if(map.Bricks[i].rotation)objectc.rotation = new Vector3(0, (map.Bricks[i].rotation-90)*Math.PI/180, 0);
        objectc.position = new Vector3(map.Bricks[i].yPos+map.Bricks[i].yScale/2, map.Bricks[i].zPos+map.Bricks[i].zScale/2, (map.Bricks[i].xPos+map.Bricks[i].xScale/2)*-1);
        if(map.Bricks[i].rot == 180 || map.Bricks[i].rot == 360){
            objectc.scale = new Vector3( map.Bricks[i].xScale/2, map.Bricks[i].zScale/2, map.Bricks[i].yScale/2);
        }else{
            objectc.scale = new Vector3( map.Bricks[i].yScale/2, map.Bricks[i].zScale/2, map.Bricks[i].xScale/2);
        }

        // objectc.texture = dot;
        objectc.shader = rainbow;
        objectc.setVec3("brick_size", objectc.scale);
        objectc.setVec3("color", map.Bricks[i].color);
        objectc.setBool("dont", false);

        objectc.create_physbody(null, null, null, 1);

        scene.push( objectc );
    }

    dev_text.text = "Kitsune Builder | " + b + " bricks";
}

var exports = {loadBrk, BV};