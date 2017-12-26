using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class toggleMode : MonoBehaviour, IPointerClickHandler {

	VRPN vrpn;
	public DeviceType type;
	private UnityEngine.UI.Toggle toogle;
	private UnityEngine.UI.Toggle[] anotherToogles;

	public GameObject[] anotherChecks;
	InputField trackerNameField;

	// Use this for initialization
	void Start () {
		vrpn = GameObject.Find ("starter").GetComponent<VRPN> ();
		trackerNameField = GameObject.Find ("trackerNameField").GetComponent<InputField>();

		toogle = GetComponent<Toggle> ();
		updateIsOn ();

		anotherToogles = new Toggle[anotherChecks.Length];
		for (int i = 0; i < anotherChecks.Length; i++) {
			anotherToogles[i] = anotherChecks[i].GetComponent<Toggle> ();
		}

		updateTrackerField ();
	}

	public void OnPointerClick(PointerEventData eventData ) {
		for (int i = 0; i < anotherToogles.Length; i++) {
			anotherToogles[i].isOn = false;
		}

		vrpn.devType = type;

		vrpn.createView ();

		updateTrackerField ();
	}
	
	// Update is called once per frame
	void updateIsOn () {
		if (vrpn.devType == type) {
			toogle.isOn = true;
		} else {
			toogle.isOn  = false;
		}
	}

	public void updateTrackerField(){
		
		if (vrpn.devType == DeviceType.LeapMotion) {
			trackerNameField.text = "LeapMotion0@localhost";
		} else {
			trackerNameField.text = "Tracker0@localhost";
		}
	}
}
