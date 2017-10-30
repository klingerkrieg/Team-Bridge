using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class toogleKinectLimit : MonoBehaviour, IPointerClickHandler {

	VRPN vrpn;
	private UnityEngine.UI.Toggle toogle;

	// Use this for initialization
	void Start () {
		vrpn = GameObject.Find ("starter").GetComponent<VRPN> ();
		toogle = GetComponent<Toggle> ();
		toogle.isOn = vrpn.kinectLimit;
	}

	public void OnPointerClick(PointerEventData eventData ) {
		vrpn.kinectLimit = toogle.isOn;
	}
}
