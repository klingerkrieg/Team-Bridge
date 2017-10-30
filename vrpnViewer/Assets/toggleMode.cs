using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class toggleMode : MonoBehaviour, IPointerClickHandler {

	VRPN vrpn;
	public bool isKinect;
	public bool isLeapMotion;
	private UnityEngine.UI.Toggle toogle;
	private UnityEngine.UI.Toggle anotherToogle;
	public GameObject anotherCheck;


	// Use this for initialization
	void Start () {
		vrpn = GameObject.Find ("starter").GetComponent<VRPN> ();
		toogle = GetComponent<Toggle> ();
		updateIsOn ();

		anotherToogle = anotherCheck.GetComponent<Toggle> ();

	}

	public void OnPointerClick(PointerEventData eventData ) {
		if (toogle.isOn) {
			anotherToogle.isOn = false;
		} else {
			anotherToogle.isOn = true;
		}

		if (isKinect) {
			vrpn.kinect = toogle.isOn;
		} else
		if (isLeapMotion){
			vrpn.kinect = !toogle.isOn;
		}
		
		vrpn.createView ();
	}
	
	// Update is called once per frame
	void updateIsOn () {
		if (isKinect) {
			toogle.isOn = vrpn.kinect;
		} else
		if (isLeapMotion){
			toogle.isOn  = !vrpn.kinect;
		}
	}
}
