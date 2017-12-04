using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using UnityEngine.UI;

public class VRPN : MonoBehaviour {

	public bool kinect;//caso leapmotion setar false;
	public bool guideON;
	public bool guide3D;
	public bool oculusON = false;

	private String oculusName = "Oculus0@localhost";

	String trackerName;
	int channels;
	int centerChannel;
	float size;
	float cyWidth;
	float guideSize = 0.08f;
	int guidePrecision = 1;
	Dictionary<Vector3, GameObject> guides = new Dictionary<Vector3, GameObject> ();
	public int guideLifeInSeconds = 2;
	Material guideMat;

	Vector3 sum = new Vector3 ();
	Vector3 lastSum = new Vector3 (0,0,0);
	bool freezed = false; 
	int lastFreezeUpdate = -1;
	bool unfreezing =  true;
	bool unfreezingRoutine = false;

	List<Bone> bones = new List<Bone> ();
	List<GameObject> spheres = new List<GameObject>();

	Vector3 centerPos;
	Vector3 headPos;
	public bool kinectLimit = true;

	InputField xIntervalField;

	[DllImport ("unityVrpn")]
	private static extern double vrpnTrackerExtern(string address, int channel, int component, int frameCount);

	public static Vector3 vrpnTrackerPos(string address, int channel){
		return new Vector3(
			(float) vrpnTrackerExtern(address, channel, 0, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 1, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 2, Time.frameCount));
	}

	public static Quaternion vrpnTrackerQuat(string address, int channel){
		return new Quaternion(
			(float) vrpnTrackerExtern(address, channel, 3, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 4, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 5, Time.frameCount),
			(float) vrpnTrackerExtern(address, channel, 6, Time.frameCount));
	}

	GameObject CreateCylinderBetweenPoints(Vector3 start, Vector3 end, float width) {
		GameObject cylinder = GameObject.CreatePrimitive (PrimitiveType.Cylinder);
		alignCylinderBetweenPoints (cylinder, start, end, width);
		return cylinder;
	}

	void alignCylinderBetweenPoints(GameObject cylinder,Vector3 start, Vector3 end, float width){
		Vector3 offset = end - start;
		Vector3 scale = new Vector3(width, (float)(offset.magnitude / 2.0), width);
		Vector3 position = start + new Vector3((float)(offset.x / 2.0), (float)(offset.y / 2.0), (float)(offset.z / 2.0));

		//Instantiate(Cylin, position, Quaternion.identity);
		cylinder.transform.position = position;
		cylinder.transform.up = offset;
		cylinder.transform.localScale = scale;

	}


	public IEnumerator moveCylinderOverTime(GameObject objectToMove, Vector3 end, Vector3 up, float seconds){

		float elapsedTime = 0;
		Vector3 startingPos = objectToMove.transform.position;
		Vector3 startingUpPos = objectToMove.transform.up;

		while (elapsedTime < seconds) {
			Vector3 newStart = Vector3.Lerp (startingPos, end, (elapsedTime / seconds));
			Vector3 newEnd = Vector3.Lerp (startingUpPos, up, (elapsedTime / seconds));

			alignCylinderBetweenPoints (objectToMove, newStart, newEnd, cyWidth);

			elapsedTime += Time.deltaTime;
			yield return new WaitForEndOfFrame();
		}

		objectToMove.transform.position = end;
		unfreezingRoutine = false;

	}


	public IEnumerator moveObjectOverTime(GameObject objectToMove, Vector3 end, float seconds){
		
		float elapsedTime = 0;
		Vector3 startingPos = objectToMove.transform.position;

		while (elapsedTime < seconds) {
			objectToMove.transform.position = Vector3.Lerp (startingPos, end, (elapsedTime / seconds));
			elapsedTime += Time.deltaTime;
			yield return new WaitForEndOfFrame();
		}

		objectToMove.transform.position = end;
		unfreezingRoutine = false;
	}

	void Start(){
		createView ();
		createLines ();

		xIntervalField = GameObject.Find ("xIntervalField").GetComponent<InputField>();
	}


	void destroyView(){
		//Create bones
		foreach (Bone bone in bones) {
			Destroy (bone.cylinder);
		}

		bones.Clear ();

		foreach (GameObject sphere in spheres) {
			Destroy(sphere);
		}

		spheres.Clear();

		//Reseta variaveis
		sum = new Vector3 ();
		lastSum = new Vector3 (0,0,0);
		freezed = false; 
		lastFreezeUpdate = -1;
		unfreezing =  true;
		unfreezingRoutine = false;
	}

	// Use this for initialization
	public void createView () {

		destroyView ();

		if (kinect) {
			//kinect
			centerChannel = 2;
			channels = 20;
			size = 0.10f;
			cyWidth = 0.06f;
			Camera.main.transform.position = new Vector3 (0f, 0f, -0.5f);

			//Kinect bones
			bones.Add (new Bone(0,1));
			bones.Add (new Bone(1,2));
			bones.Add (new Bone(2,3));
			//left arm
			bones.Add (new Bone(1,8));
			bones.Add (new Bone(8,9));
			bones.Add (new Bone(9,10));
			bones.Add (new Bone(10, 11));
			//right arm
			bones.Add (new Bone(1,4));
			bones.Add (new Bone(4,5));
			bones.Add (new Bone(5,6));
			bones.Add (new Bone(6, 7));
			//left leg
			bones.Add (new Bone(3,16));
			bones.Add (new Bone(16,17));
			bones.Add (new Bone(17,18));
			bones.Add (new Bone(18, 19));
			//right leg
			bones.Add (new Bone(3,12));
			bones.Add (new Bone(12,13));
			bones.Add (new Bone(13,14));
			bones.Add (new Bone(14, 15));

		} else {

			//Leap bones
			//mao 1
			bones.Add (new Bone(0,2));
			bones.Add (new Bone(1,2));
			//polegar
			bones.Add (new Bone(2,3));
			bones.Add (new Bone(3,4));
			bones.Add (new Bone(4,5));
			bones.Add (new Bone(5,6));
			//indicador
			bones.Add (new Bone(2,7));
			bones.Add (new Bone(7,8));
			bones.Add (new Bone(8,9));
			bones.Add (new Bone(9,10));
			//meio
			bones.Add (new Bone(2,11));
			bones.Add (new Bone(11,12));
			bones.Add (new Bone(12,13));
			bones.Add (new Bone(13,14));
			//medio
			bones.Add (new Bone(2,15));
			bones.Add (new Bone(15,16));
			bones.Add (new Bone(16,17));
			bones.Add (new Bone(17,18));
			//mindinho
			bones.Add (new Bone(2,19));
			bones.Add (new Bone(19,20));
			bones.Add (new Bone(20,21));
			bones.Add (new Bone(21,22));

			//Mao 2
			bones.Add (new Bone(23,25));
			bones.Add (new Bone(24,25));
			//polegar
			bones.Add (new Bone(25,26));
			bones.Add (new Bone(26,27));
			bones.Add (new Bone(27,28));
			bones.Add (new Bone(28,29));
			//indicador
			bones.Add (new Bone(25,30));
			bones.Add (new Bone(30,31));
			bones.Add (new Bone(31,32));
			bones.Add (new Bone(32,33));
			//meio
			bones.Add (new Bone(25,34));
			bones.Add (new Bone(34,35));
			bones.Add (new Bone(35,36));
			bones.Add (new Bone(36,37));
			//medio
			bones.Add (new Bone(25,38));
			bones.Add (new Bone(38,39));
			bones.Add (new Bone(39,40));
			bones.Add (new Bone(40,41));
			//mindinho
			bones.Add (new Bone(25,42));
			bones.Add (new Bone(42,43));
			bones.Add (new Bone(43,44));
			bones.Add (new Bone(44,45));

			//leapmotion
			centerChannel = 0;
			channels = 46;
			size = 15f;
			cyWidth = 5f;
			Camera.main.transform.position = new Vector3 (0f, 15f, -349.2f);
		}

		Material newMat = Resources.Load("esfera", typeof(Material)) as Material;
		guideMat = Resources.Load("guide", typeof(Material)) as Material;


		if (!kinect) {
			trackerName = "LeapMotion0@localhost";
		} else {
			trackerName = "Tracker0@localhost";
		}

		//Create bones
		foreach (Bone bone in bones) {
			
			Vector3 start = vrpnTrackerPos (trackerName, bone.sensStart);
			Vector3 end = vrpnTrackerPos (trackerName, bone.sensEnd);
			
			bone.cylinder = CreateCylinderBetweenPoints (start,end,cyWidth);
		}

		for (int i = 0; i < channels; i++) {
			GameObject sphere = GameObject.CreatePrimitive (PrimitiveType.Sphere);
			sphere.name = i.ToString();
			sphere.transform.localScale = new Vector3 (size,size,size);
			MeshRenderer mesh = sphere.GetComponent<MeshRenderer>();
			mesh.material = newMat;

			spheres.Add (sphere);
		}


	}


	Vector3 ceilPosition(Vector3 vec){
		return new Vector3( (float)Math.Round(vec.x, guidePrecision), (float)Math.Round (vec.y, guidePrecision), (float)Math.Round (vec.z, guidePrecision));
	}

	public void removeFromGuide(Vector3 vec){
		guides.Remove (vec);
	}


	void OnGUI(){
		GUI.color = Color.red;
		Vector3 screenPos;

		//Caso esteja montando
		if (unfreezingRoutine) {
			return;
		}

		foreach (Bone bone in bones) {
			if (freezed == false) {
				Vector3 start = vrpnTrackerPos (trackerName, bone.sensStart);
				Vector3 end = vrpnTrackerPos (trackerName, bone.sensEnd);

				if (unfreezing) {
					//se estiver montando fara isso em tempo nao de uma unica vez
					StartCoroutine(moveCylinderOverTime(bone.cylinder, start, end, 0.5f));
					unfreezingRoutine = true;
				} else {
					alignCylinderBetweenPoints (bone.cylinder, start, end, cyWidth);
				}

				Destroy(bone.cylinder.GetComponent<Rigidbody>());
			} else {
				if (bone.cylinder.GetComponent<Rigidbody>() == null)
					bone.cylinder.AddComponent<Rigidbody>();
			}
		}

		//Contator para verificar se parou de receber dados
		sum = new Vector3 (0, 0, 0);

		int i = -1;
		GameObject central = null;
		foreach (GameObject sphere in spheres){
			i++;
			Vector3 pos = vrpnTrackerPos (trackerName, i);
			sum += pos;

			if (i == 0) {
				headPos = pos;
			} else
			if (i == 3) {
				centerPos = pos;
			}



			if (freezed == false) {
				Destroy(sphere.GetComponent<Rigidbody>());

				if (unfreezing) {
					StartCoroutine(moveObjectOverTime(sphere, pos, 0.5f));
					unfreezingRoutine = true;
				} else {
					sphere.transform.position = pos;
				}


				if (guideON && (i == 7 || i == 11)) {
					Vector3 guidePos;
					if (guide3D) {
						//Cria guia 3d
						guidePos = ceilPosition (sphere.transform.position);
					} else {
						//Cria guia 2d
						guidePos = ceilPosition (sphere.transform.position);
						guidePos.z = 1;
					}

					if (!guides.ContainsKey (guidePos)) {
						GameObject guide = GameObject.CreatePrimitive (PrimitiveType.Cube);
						guide.transform.localScale = new Vector3 (guideSize, guideSize, guideSize);
						guide.transform.position = guidePos;
						MeshRenderer mesh = guide.GetComponent<MeshRenderer>();
						mesh.material = guideMat;
						Timeout to = guide.AddComponent<Timeout> ();
						to.timeout = guideLifeInSeconds;
						guides.Add (guidePos, guide);
					}
				}



			} else {
				if (sphere.GetComponent<Rigidbody>() == null)
					sphere.AddComponent<Rigidbody>();
			}


			screenPos = Camera.main.WorldToScreenPoint(sphere.transform.position);
			//nao sei porque o x se comporta corretamente e o y comeca de baixo pra cima
			GUI.Label (new Rect(screenPos.x, Camera.main.pixelHeight - screenPos.y,20,20), sphere.name);

			if (i == centerChannel)
				central = sphere;

		}

		//se estava descongelando
		if (unfreezing) {
			//nao esta mais
			unfreezing = false;
		}

		//Verifica se parou de receber do kinect
		System.DateTime epochStart = new System.DateTime(1970, 1, 1, 0, 0, 0, System.DateTimeKind.Utc);
		int now = (int)(System.DateTime.UtcNow - epochStart).TotalSeconds;

		if (lastFreezeUpdate == -1 || now - lastFreezeUpdate > 0.5f) {
			lastFreezeUpdate = now;
			if (sum == lastSum) {
				freezed = true;
			} else {
				if (freezed)
					unfreezing = true;
				freezed = false;
			}
			lastSum = sum;

		}


		if (oculusON == false) {
			if (freezed == false) {
				if (central != null)
					Camera.main.transform.LookAt (central.transform);
			}
		} else {
			Vector3 pos = vrpnTrackerPos (oculusName, 0);
			Quaternion quat = vrpnTrackerQuat (oculusName, 0);
			print (quat);
			Camera.main.transform.position = pos;
			//Camera.main.transform.rotation = quat;
			Camera.main.transform.localRotation = quat;
		}


	}
	
	// Update is called once per frame
	/*void OnDrawGizmos () {
		Debug.Log ("HERE");
		Vector3 leftLimit = headPos;
		leftLimit.y = 40;
		//leftLimit.x += 0.10f;
		Vector3 leftDown = leftLimit;
		leftDown.y = -40;

		Gizmos.color = Color.blue;
		Gizmos.DrawLine(leftLimit, leftDown);
	}*/

	private LineRenderer line;


	void createLines(){
		line = GetComponent<LineRenderer> ();
	}

	void Update() {
		if (kinect && kinectLimit) {
			float handTopInterval = 0.10f;
			float handXInterval;


			if (!float.TryParse(xIntervalField.text, out handXInterval)) {
				handXInterval = 0.40f;
			}


			float leftLimit = centerPos.x - (handXInterval*3);
			float rightLimit = centerPos.x + (handXInterval*3);
			float topLimit = headPos.y + 1;



			float top5 = headPos.y + (handTopInterval * 5);
			float top4 = headPos.y + (handTopInterval * 2);
			float top3 = headPos.y + handTopInterval;
			float top2 = headPos.y - (handTopInterval * 2);
			float top1 = headPos.y - (handTopInterval * 5);
			float top0 = -0.95f;

			Vector3[] positions = new Vector3[26];
			//Left Up
			positions [0] = headPos;
			positions [0].x = centerPos.x + (handXInterval);
			positions [0].y = top5;

			//Left down
			positions [1] = positions [0];
			positions [1].y = top0;


			//Right down
			positions [2] = headPos;
			positions [2].x = centerPos.x - (handXInterval);
			positions [2].y = top0;

			//Right up
			positions [3] = positions [2];
			positions [3].y = top5;


			//Top 5
			positions [4] = headPos;
			positions [4].y = top5;
			positions [4].x = leftLimit;

			positions [5] = positions [4];
			positions [5].x = rightLimit;


			//Top 4
			positions [6] = headPos;
			positions [6].y = top4;
			positions [6].x = rightLimit;

			positions [7] = positions [6];
			positions [7].x = leftLimit;

			positions [8] = positions [7];
			positions [8].y = top5;

			//Top 3
			positions [9] = headPos;
			positions [9].y = top3;
			positions [9].x = leftLimit;

			positions [10] = positions [9];
			positions [10].x = rightLimit;

			positions [11] = positions [10];
			positions [11].y = top4;

			//Top 2
			positions [12] = headPos;
			positions [12].y = top2;
			positions [12].x = rightLimit;

			positions [13] = positions [12];
			positions [13].x = leftLimit;

			positions [14] = positions [13];
			positions [14].y = top3;

			//Top 1
			positions [15] = headPos;
			positions [15].y = top1;
			positions [15].x = leftLimit;

			positions [16] = positions [15];
			positions [16].x = rightLimit;

			positions [17] = positions [16];
			positions [17].y = top2;

			//Top 0
			positions [18] = positions [17];
			positions [18].y = top0;

			positions [19] = positions [18];
			positions [19].x = leftLimit;

			positions [20] = positions [16];
			positions [20] = positions [15];

			positions [21] = positions [20];
			positions [21].x = centerPos.x + (handXInterval*2);

			positions [22] = positions [21];
			positions [22].y = top0;

			positions [23] = positions [22];
			positions [23].y = top5;

			positions [24] = positions [23];
			positions [24].x = centerPos.x - (handXInterval*2);

			positions [25] = positions [24];
			positions [25].y = top0;

			line.SetPositions (positions);
			line.enabled = true;
		} else {
			line.enabled = false;
		}
	}
}
