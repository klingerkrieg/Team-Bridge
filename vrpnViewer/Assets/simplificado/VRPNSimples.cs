using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

public class VRPNSimples : MonoBehaviour {

	int qtdSensores = 20;
	float size = 0.15f;	
	List<GameObject> spheres = new List<GameObject>();

	[DllImport ("unityVrpn")]
	private static extern double vrpnTrackerExtern(string address, int channel, int component, int frameCount);

	public static Vector3 vrpnTrackerPos(string address, int channel){
		return new Vector3(
			(float) vrpnTrackerExtern(address, channel, 0, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 1, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 2, Time.frameCount));
	}


	void criarBolinhas(){
		for (int i = 0; i < qtdSensores; i++) {
			GameObject sphere = GameObject.CreatePrimitive (PrimitiveType.Sphere);
			sphere.name = i.ToString();
			//tamanho
			sphere.transform.localScale = new Vector3 (size,size,size);
			//adiciono na lista de esferas
			spheres.Add (sphere);
		}
	}


	void atualizarPosicaoDasBolas(){
		int i = 0;
		foreach (GameObject sphere in spheres){
			Vector3 pos = vrpnTrackerPos ("Tracker0@localhost", i);
			sphere.transform.position = pos;
			i++;
		}
	}
	

	// Use this for initialization
	void Start () {
		criarBolinhas();
	}
	
	// Update is called once per frame
	void Update () {
		atualizarPosicaoDasBolas();

		if (Input.GetKeyDown(KeyCode.Space)){
			//Debug.Log(spheres[7].transform.position);
			Debug.Log(spheres[11].transform.position);
		}
	}
}
