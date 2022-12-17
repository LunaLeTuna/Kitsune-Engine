#pragma once

void hotload(){
    for(int i = 0; cshaders.size() > i; i++){
        string vt = get_file_date(cshaders[i].Location+".vert");

        string ft = get_file_date(cshaders[i].Location+".frag");

        if(cshaders[i].Vlast_changed.compare(vt) != 0 || cshaders[i].Flast_changed.compare(ft) != 0){
            cshaders[i].Act_Hotswap(cshaders[i].Location, vt, ft);
            //cout << "nya~ " << cshaders[i].Location << " changed :3" << endl;
        }
    }
}