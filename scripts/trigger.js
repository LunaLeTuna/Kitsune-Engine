class Trigger {
    constructor(pos,sca,rot,wado) {
        //this._KE_Prop = l ?? create_prop();
        this.position = new Vector3(0,0,0)
        this.scale = new Vector3(0,0,0)
        this.rotation = new Vector3(0,0,0)
    }

    test(wado){
        var hafedscale = new Vector3(this.scale.x/2,this.scale.y/2,this.scale.z/2)
        var startpoint = new Vector3(this.position.x-hafedscale.x,this.position.y-hafedscale.y,this.position.z-hafedscale.z)
        if(true){

        }
    }
}