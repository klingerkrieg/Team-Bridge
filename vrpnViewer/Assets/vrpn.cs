using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;

public class vrpn : MonoBehaviour {

	bool kinect = true;//caso leapmotion setar false;
	
	int channels;
	int centerChannel;
	float size;

	List<GameObject> spheres = new List<GameObject>();

	[DllImport ("unityVrpn")]
	private static extern double vrpnTrackerExtern(string address, int channel, int component, int frameCount);

	public static Vector3 vrpnTrackerPos(string address, int channel){
		return new Vector3(
			(float) vrpnTrackerExtern(address, channel, 0, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 1, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 2, Time.frameCount));
	}

	// Use this for initialization
	void Start () {

		if (kinect) {
			//kinect
			centerChannel = 2;
			channels = 20;
			size = 0.1f;
			Camera.main.transform.position = new Vector3 (0f, 0f, -0.5f);
		} else {
			//leapmotion
			centerChannel = 0;
			channels = 46;
			size = 20f;
			Camera.main.transform.position = new Vector3 (0f, 15f, -349.2f);
		}

		Material newMat = Resources.Load("verde", typeof(Material)) as Material;


		for (int i = 0; i < channels; i++) {
			GameObject sphere = GameObject.CreatePrimitive (PrimitiveType.Sphere);
			sphere.name = i.ToString();
			sphere.transform.localScale = new Vector3 (size,size,size);
			MeshRenderer mesh = sphere.GetComponent<MeshRenderer>();
			mesh.material = newMat;
			spheres.Add (sphere);

		}
	}

	void OnGUI(){
		GUI.color = Color.red;
		Vector3 screenPos;



		int i = 0;
		GameObject central = null;
		foreach (GameObject sphere in spheres){
			sphere.transform.position = vrpnTrackerPos ("Tracker0@localhost", i++);


			screenPos = Camera.main.WorldToScreenPoint(sphere.transform.position);
			//nao sei porque o x se comporta corretamente e o y comeca de baixo pra cima
			GUI.Label (new Rect(screenPos.x, Camera.main.pixelHeight - screenPos.y,20,20), sphere.name);

			if (i == centerChannel)
				central = sphere;
		}

		if (central != null)
			Camera.main.transform.LookAt (central.transform);
	}
	
	// Update is called once per frame
	void Update () {
		

	}
}
