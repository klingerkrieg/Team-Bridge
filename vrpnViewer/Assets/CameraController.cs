using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour {

	VRPN vrpn;
	int sens = 10;
	// Use this for initialization
	void Start () {
		vrpn = GameObject.Find ("starter").GetComponent<VRPN> ();
	}
	
	// Update is called once per frame
	void Update () {
		if (Input.GetKey (KeyCode.LeftArrow)) {
			this.transform.Rotate ( this.transform.up *-1 , Time.deltaTime*sens);
			vrpn.cameraAuto = false;
		} else
		if (Input.GetKey (KeyCode.RightArrow)) {
			this.transform.Rotate ( this.transform.up , Time.deltaTime*sens);
			vrpn.cameraAuto = false;
		}
		if (Input.GetKey (KeyCode.UpArrow)) {
			this.transform.Rotate ( this.transform.right *-1 , Time.deltaTime*sens);
			vrpn.cameraAuto = false;
		} else
		if (Input.GetKey (KeyCode.DownArrow)) {
			this.transform.Rotate ( this.transform.right, Time.deltaTime*sens );
			vrpn.cameraAuto = false;
		} 
	
	}
}
