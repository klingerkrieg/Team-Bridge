using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bone {

	public GameObject cylinder;

	public int sensStart;
	public int sensEnd;

	public Bone(int sensStart, int sensEnd){
		this.sensStart = sensStart;
		this.sensEnd = sensEnd;
	}

}
