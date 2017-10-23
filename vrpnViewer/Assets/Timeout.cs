using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Timeout : MonoBehaviour {

	public int timeout = 5;
	int started;

	// Use this for initialization
	void Start () {
		System.DateTime epochStart = new System.DateTime(1970, 1, 1, 0, 0, 0, System.DateTimeKind.Utc);
		started = (int)(System.DateTime.UtcNow - epochStart).TotalSeconds;
	}
	
	// Update is called once per frame
	void Update () {

		System.DateTime epochStart = new System.DateTime(1970, 1, 1, 0, 0, 0, System.DateTimeKind.Utc);
		int now = (int)(System.DateTime.UtcNow - epochStart).TotalSeconds;
		if (now - started > timeout) {
			//Remove do vetor para poder inserir novamente caso seja necessario
			VRPN vrpn = GameObject.Find ("starter").GetComponent<VRPN> ();
			vrpn.removeFromGuide (this.gameObject.transform.position);
			Destroy (this.gameObject);
		}
	}
}
