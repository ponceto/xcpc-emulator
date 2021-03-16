#include "config.h"
#ifdef HAVE_JNI_H
#include <stdlib.h>
#include <jni.h>
#include "libdsk.h"
#include "uk_co_demon_seasip_libdsk_LibDsk.h"
#include "uk_co_demon_seasip_libdsk_Drive.h"
#include "uk_co_demon_seasip_libdsk_Geometry.h"
#include "uk_co_demon_seasip_libdsk_FormatType.h"

/***************************** General utilities ****************************/

/* Convert LibDsk error to a LibDsk exception */
static int check_error(JNIEnv *env, dsk_err_t error)
{
	jthrowable excep;
	jmethodID mid;
	jclass clazz;
	jstring str;

	if (error == DSK_ERR_OK) return 0;

	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/DskException");
	if (clazz == NULL) return 0;

	mid = (*env)->GetMethodID(env, clazz, "<init>", "(Ljava/lang/String;I)V");
	if (mid == NULL) return 0;

	str = (*env)->NewStringUTF(env, dsk_strerror(error));
	if (str == NULL) return 0;

	excep = (*env)->NewObject(env, clazz, mid, str, (jint)error);

	(*env)->Throw(env, excep);	
	return 1;	
}

/* Note that entry 0 must always be NULL */
static jint add_map(JNIEnv *env, DSK_PDRIVER ptr)
{
	dsk_err_t err;
	unsigned int n;

	err = dsk_map_dtoi(ptr, &n);
	if (err)
	{
		check_error(env, err);
		return 0;
	}
	return n;		
}


static jobject driver_new(JNIEnv *env, DSK_PDRIVER ptr)
{
	jclass clazz;
	jmethodID mid;
	jint drvid;

	drvid = add_map(env, ptr);
	if (!drvid) return NULL;
	
	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/Drive");
	if (clazz == NULL) return 0;
	mid = (*env)->GetMethodID(env, clazz, "<init>", "(I)V");
	if (mid == NULL) return 0;
	return (*env)->NewObject(env, clazz, mid, drvid);
}


static void driver_delete(JNIEnv *env, jobject self)
{
	jint drvid;
	jclass clazz;
	jfieldID fid;

	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/Drive");
	if (!clazz) { check_error(env, DSK_ERR_BADPTR); return; }

	fid = (*env)->GetFieldID(env, clazz, "id", "I");
	if (!fid) { check_error(env, DSK_ERR_BADPTR); return; }
	drvid = (*env)->GetIntField(env, self, fid);	

	dsk_map_delete(drvid);	
	(*env)->SetIntField(env, self, fid, 0);
}


static DSK_PDRIVER driver_from_java(JNIEnv *env, jobject obj)
{	
	jfieldID fid;
	jclass clazz;
	jint drvid;
	dsk_err_t err;
	DSK_PDRIVER ptr;

	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/Drive");
	if (!clazz) { check_error(env, DSK_ERR_BADPTR); return NULL; }

	fid = (*env)->GetFieldID(env, clazz, "id", "I");
	if (!fid) { check_error(env, DSK_ERR_BADPTR); return NULL; }
	drvid = (*env)->GetIntField(env, obj, fid);	

	err = dsk_map_itod(drvid, &ptr);	
	if (err)	
	{
		check_error(env, err);
		return NULL;
	} 
	return ptr;
}


/************************* Geometry conversion ****************************/

static dsk_err_t geom_from_java  (JNIEnv *env, jobject geom, DSK_GEOMETRY *dg)
{
	jclass clazz;
	jfieldID fid;

	if (!dg) return DSK_ERR_BADPTR;
	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/Geometry");
	if (!clazz) return DSK_ERR_BADPTR;

	fid = (*env)->GetFieldID(env, clazz, "sidedness", "I"); if (!fid) return DSK_ERR_BADPTR; dg->dg_sidedness = (*env)->GetIntField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "cylinders", "I"); if (!fid) return DSK_ERR_BADPTR; dg->dg_cylinders = (*env)->GetIntField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "heads",     "I"); if (!fid) return DSK_ERR_BADPTR; dg->dg_heads     = (*env)->GetIntField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "sectors",   "I"); if (!fid) return DSK_ERR_BADPTR; dg->dg_sectors   = (*env)->GetIntField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "secbase",   "I"); if (!fid) return DSK_ERR_BADPTR; dg->dg_secbase   = (*env)->GetIntField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "secsize",   "I"); if (!fid) return DSK_ERR_BADPTR; dg->dg_secsize   = (*env)->GetIntField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "datarate",  "I"); if (!fid) return DSK_ERR_BADPTR; dg->dg_datarate  = (*env)->GetIntField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "rwgap",     "B"); if (!fid) return DSK_ERR_BADPTR; dg->dg_rwgap     = (*env)->GetByteField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "fmtgap",    "B"); if (!fid) return DSK_ERR_BADPTR; dg->dg_fmtgap    = (*env)->GetByteField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "fm",        "Z"); if (!fid) return DSK_ERR_BADPTR; dg->dg_fm        = (*env)->GetBooleanField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "nomulti",   "Z"); if (!fid) return DSK_ERR_BADPTR; dg->dg_nomulti   = (*env)->GetBooleanField(env, geom, fid);
	fid = (*env)->GetFieldID(env, clazz, "noskip",    "Z"); if (!fid) return DSK_ERR_BADPTR; dg->dg_noskip    = (*env)->GetBooleanField(env, geom, fid);
	return DSK_ERR_OK;
}



static dsk_err_t geom_to_java  (JNIEnv *env, jobject geom, DSK_GEOMETRY *dg)
{
	jclass clazz;
	jfieldID fid;

	if (!dg) return DSK_ERR_BADPTR;
	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/Geometry");
	if (!clazz) return DSK_ERR_BADPTR;

	fid = (*env)->GetFieldID(env, clazz, "sidedness", "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, geom, fid, dg->dg_sidedness);
	fid = (*env)->GetFieldID(env, clazz, "cylinders", "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, geom, fid, dg->dg_cylinders);
	fid = (*env)->GetFieldID(env, clazz, "heads",     "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, geom, fid, dg->dg_heads);
	fid = (*env)->GetFieldID(env, clazz, "sectors",   "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, geom, fid, dg->dg_sectors);
	fid = (*env)->GetFieldID(env, clazz, "secbase",   "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, geom, fid, dg->dg_secbase);
	fid = (*env)->GetFieldID(env, clazz, "secsize",   "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, geom, fid, dg->dg_secsize);
	fid = (*env)->GetFieldID(env, clazz, "datarate",  "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, geom, fid, dg->dg_datarate);
	fid = (*env)->GetFieldID(env, clazz, "rwgap",     "B"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetByteField(env, geom, fid, dg->dg_rwgap);
	fid = (*env)->GetFieldID(env, clazz, "fmtgap",    "B"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetByteField(env, geom, fid, dg->dg_fmtgap);
	fid = (*env)->GetFieldID(env, clazz, "fm",        "Z"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetBooleanField(env, geom, fid, dg->dg_fm);
	fid = (*env)->GetFieldID(env, clazz, "nomulti",   "Z"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetBooleanField(env, geom, fid, dg->dg_nomulti);
	fid = (*env)->GetFieldID(env, clazz, "noskip",    "Z"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetBooleanField(env, geom, fid, dg->dg_noskip);
	return DSK_ERR_OK;
}


/************************* Format conversion ****************************/

static dsk_err_t format_from_java  (JNIEnv *env, jobject fmt, DSK_FORMAT *df)
{
	jclass clazz;
	jfieldID fid;

	if (!fmt) return DSK_ERR_BADPTR;
	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/FormatData");
	if (!clazz) return DSK_ERR_BADPTR;

	fid = (*env)->GetFieldID(env, clazz, "cylinder", "I"); if (!fid) return DSK_ERR_BADPTR; df->fmt_cylinder = (*env)->GetIntField(env, fmt, fid);
	fid = (*env)->GetFieldID(env, clazz, "head",     "I"); if (!fid) return DSK_ERR_BADPTR; df->fmt_head     = (*env)->GetIntField(env, fmt, fid);
	fid = (*env)->GetFieldID(env, clazz, "sector",   "I"); if (!fid) return DSK_ERR_BADPTR; df->fmt_sector   = (*env)->GetIntField(env, fmt, fid);
	fid = (*env)->GetFieldID(env, clazz, "secsize",  "I"); if (!fid) return DSK_ERR_BADPTR; df->fmt_secsize  = (*env)->GetIntField(env, fmt, fid);
	return DSK_ERR_OK;
}

static dsk_err_t format_to_java  (JNIEnv *env, jobject fmt, DSK_FORMAT *df)
{
	jclass clazz;
	jfieldID fid;

	if (!fmt) return DSK_ERR_BADPTR;
	clazz = (*env)->FindClass(env, "uk/co/demon/seasip/libdsk/FormatData");
	if (!clazz) return DSK_ERR_BADPTR;

	fid = (*env)->GetFieldID(env, clazz, "cylinder", "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, fmt, fid, df->fmt_cylinder);
	fid = (*env)->GetFieldID(env, clazz, "head",     "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, fmt, fid, df->fmt_head);
	fid = (*env)->GetFieldID(env, clazz, "sector",   "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, fmt, fid, df->fmt_sector);
	fid = (*env)->GetFieldID(env, clazz, "secsize",  "I"); if (!fid) return DSK_ERR_BADPTR; (*env)->SetIntField(env, fmt, fid, df->fmt_secsize);
	return DSK_ERR_OK;
}




/***************************** The LibDsk class ****************************/

/* Wrap dsk_get_psh() */
JNIEXPORT jbyte JNICALL Java_uk_co_demon_seasip_libdsk_LibDsk_getPsh
  (JNIEnv *env, jclass clazz, jint size)
{
	return dsk_get_psh(size);
}


static jobject open_or_create
  (JNIEnv *env, jclass clazz, jstring filename, jstring type, jstring comp, int create)
{
	const char *utfname = NULL;
	const char *utftype = NULL;
	const char *utfcomp = NULL;
	dsk_err_t error;	
	DSK_PDRIVER ptr;

	if (filename != NULL) 
		utfname = (*env)->GetStringUTFChars(env, filename, NULL);
	if (type != NULL)
		utftype = (*env)->GetStringUTFChars(env, type, NULL);
	if (comp != NULL)
		utfcomp = (*env)->GetStringUTFChars(env, comp, NULL);
	
	if (create) error = dsk_creat(&ptr, utfname, utftype, utfcomp);
	else	    error = dsk_open (&ptr, utfname, utftype, utfcomp);

	if (utfname != NULL) 
		(*env)->ReleaseStringUTFChars(env, filename, utfname);
	if (utftype != NULL) 
		(*env)->ReleaseStringUTFChars(env, type, utftype);
	if (utfcomp != NULL) 
		(*env)->ReleaseStringUTFChars(env, comp, utfcomp);
		
	if (check_error(env, error)) return NULL;

	/* Drive opened. Create the Java wrapper object */	
	return driver_new(env, ptr);
}
/*
 * Class:     uk_co_demon_seasip_libdsk_LibDsk
 * Method:    open
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Luk/co/demon/seasip/libdsk/Drive;
 */
JNIEXPORT jobject JNICALL Java_uk_co_demon_seasip_libdsk_LibDsk_open
  (JNIEnv *env, jclass clazz, jstring filename, jstring type, jstring comp)
{
	return open_or_create(env, clazz, filename, type, comp, 0);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_LibDsk
 * Method:    create
 * Signature: (Ljava/lang/String;Ljava/lang/String;)Luk/co/demon/seasip/libdsk/Drive;
 */
JNIEXPORT jobject JNICALL Java_uk_co_demon_seasip_libdsk_LibDsk_create
  (JNIEnv *env, jclass clazz, jstring filename, jstring type, jstring comp)
{
	return open_or_create(env, clazz, filename, type, comp, 1);
}


/* LibDsk-to-Java callbacks */
static JNIEnv  *cbEnv;

static void jni_report(const char *s)
{
	jclass clazz;
	jmethodID mid;
	jstring str = (*cbEnv)->NewStringUTF(cbEnv, s);

	clazz= (*cbEnv)->FindClass(cbEnv, "uk/co/demon/seasip/libdsk/LibDsk");
	if (!clazz) return;
	mid = (*cbEnv)->GetStaticMethodID(cbEnv, clazz, "report", "(Ljava/lang/String;)V");
	if (!mid) return;
	(*cbEnv)->CallStaticVoidMethod(cbEnv, clazz, mid, str);
}


static void jni_report_end()
{
	jclass clazz;
	jmethodID mid;

	clazz= (*cbEnv)->FindClass(cbEnv, "uk/co/demon/seasip/libdsk/LibDsk");
	if (!clazz) return;
	mid = (*cbEnv)->GetStaticMethodID(cbEnv, clazz, "reportEnd", "()V");
	if (!mid) return;
	(*cbEnv)->CallStaticVoidMethod(cbEnv, clazz, mid);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_LibDsk
 * Method:    setReporter
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_LibDsk_setReporter
  (JNIEnv *env, jclass clazz, jboolean active)
{
	cbEnv = env;

	if (active) dsk_reportfunc_set(jni_report, jni_report_end);
	else	    dsk_reportfunc_set(NULL, NULL);
}



/****************************** The Drive class *****************************/

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    setForceHead
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_setForceHead
  (JNIEnv *env, jobject self, jint force)
{
	DSK_PDRIVER drv = driver_from_java(env, self);
	
	dsk_set_forcehead(drv, force);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    getForceHead
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_uk_co_demon_seasip_libdsk_Drive_getForceHead
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER drv = driver_from_java(env, self);
	int force;
	
	dsk_get_forcehead(drv, &force);	
	return force;
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    close
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_close
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER drv = driver_from_java(env, self);
	dsk_close(&drv);	
	driver_delete(env, self);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    status
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;I)B
 */
JNIEXPORT jbyte JNICALL Java_uk_co_demon_seasip_libdsk_Drive_status
  (JNIEnv *env, jobject self, jobject jg, jint head)
{	
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	unsigned char status;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	if (!err) err = dsk_drive_status(drv, &dg, head, &status);
	check_error(env, err);
	return status;
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    read
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BIII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_read__Luk_co_demon_seasip_libdsk_Geometry_2_3BIII
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint cyl, jint head, jint sec)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_pread(drv, &dg, buf, cyl, head, sec);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);

}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    read
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BI)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_read__Luk_co_demon_seasip_libdsk_Geometry_2_3BI
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint lsec)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_lread(drv, &dg, buf, lsec);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    read
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BIIIIII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_read__Luk_co_demon_seasip_libdsk_Geometry_2_3BIIIIII
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint cyl, jint head, 
		jint cyl_expected, jint head_expected, jint sector, jint sector_len,
		jarray jdeleted)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	jboolean *bools;
	int deleted;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	bools = (*env)->GetBooleanArrayElements(env, jdeleted, NULL);
	deleted = bools[0];
	if (!err) err = dsk_xread(drv, &dg, buf, cyl, head, cyl_expected, head_expected, sector, sector_len, &deleted);
	bools[0] = deleted;
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	(*env)->ReleaseByteArrayElements(env, jdeleted, bools, 0);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    write
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BIII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_write__Luk_co_demon_seasip_libdsk_Geometry_2_3BIII
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint cyl, jint head, jint sec)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_pwrite(drv, &dg, buf, cyl, head, sec);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    write
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BI)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_write__Luk_co_demon_seasip_libdsk_Geometry_2_3BI
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint lsec)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_lwrite(drv, &dg, buf, lsec);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    write
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BIIIIII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_write__Luk_co_demon_seasip_libdsk_Geometry_2_3BIIIIII
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint cyl, jint head, 
		jint cyl_expected, jint head_expected, jint sector, jint sector_len,
		jboolean deleted)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_xwrite(drv, &dg, buf, cyl, head, cyl_expected, head_expected, sector, sector_len, deleted);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    verify
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BIII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_verify__Luk_co_demon_seasip_libdsk_Geometry_2_3BIII
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint cyl, jint head, jint sec)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_pcheck(drv, &dg, buf, cyl, head, sec);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}



/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    verify
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BI)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_verify__Luk_co_demon_seasip_libdsk_Geometry_2_3BI
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint lsec)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_lcheck(drv, &dg, buf, lsec);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    verify
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BIIIIII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_verify__Luk_co_demon_seasip_libdsk_Geometry_2_3BIIIIII
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint cyl, jint head, 
		jint cyl_expected, jint head_expected, jint sector, jint sector_len)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_xcheck(drv, &dg, buf, cyl, head, cyl_expected, head_expected, sector, sector_len);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    format
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;IILuk/co/demon/seasip/libdsk/FormatData;B)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_format__Luk_co_demon_seasip_libdsk_Geometry_2IILuk_co_demon_seasip_libdsk_FormatData_2B
  (JNIEnv *env, jobject self, jobject jg, jint cyl, jint head, jobjectArray spec, jbyte filler)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	DSK_FORMAT   *fmt = NULL;
	dsk_err_t err;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);
	if (!err) 
	{
		fmt = malloc(dg.dg_sectors * sizeof(DSK_FORMAT));
		if (fmt == NULL) err = DSK_ERR_NOMEM;
	}
	if (!err) 
	{
		int n;
		for (n = 0; n < dg.dg_sectors;n++)
		{
			jobject jfmt = (*env)->GetObjectArrayElement(env, spec, n);
			err = format_from_java(env, jfmt, &fmt[n]);
			if (err) break;
		}
	}
	if (!err) err = dsk_pformat(drv, &dg, cyl, head, fmt, filler);
	if (!err) err = geom_to_java(env, jg, &dg);
	if (fmt) free(fmt);
	check_error(env, err);
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    format
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;ILuk/co/demon/seasip/libdsk/FormatData;B)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_format__Luk_co_demon_seasip_libdsk_Geometry_2ILuk_co_demon_seasip_libdsk_FormatData_2B
  (JNIEnv *env, jobject self, jobject jg, jint track, jobjectArray spec, jbyte filler)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	DSK_FORMAT   *fmt = NULL;
	dsk_err_t err;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);
	if (!err) 
	{
		fmt = malloc(dg.dg_sectors * sizeof(DSK_FORMAT));
		if (fmt == NULL) err = DSK_ERR_NOMEM;
	}
	if (!err) 
	{
		int n;
		for (n = 0; n < dg.dg_sectors;n++)
		{
			jobject jfmt = (*env)->GetObjectArrayElement(env, spec, n);
			err = format_from_java(env, jfmt, &fmt[n]);
			if (err) break;
		}
	}
	if (!err) err = dsk_lformat(drv, &dg, track, fmt, filler);
	if (!err) err = geom_to_java(env, jg, &dg);
	if (fmt) free(fmt);
	check_error(env, err);
}
/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    readTrack
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BIIII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_readTrack__Luk_co_demon_seasip_libdsk_Geometry_2_3BIIII
  (JNIEnv *, jobject, jobject, jbyteArray, jint, jint, jint, jint);

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    readTrack
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BII)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_readTrack__Luk_co_demon_seasip_libdsk_Geometry_2_3BII
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint cyl, jint head)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_ptread(drv, &dg, buf, cyl, head);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    readTrack
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;[BI)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_readTrack__Luk_co_demon_seasip_libdsk_Geometry_2_3BI
  (JNIEnv *env, jobject self, jobject jg, jbyteArray arr, jint track)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dsk_ltread(drv, &dg, buf, track);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    autoFormat
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;IIB)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_autoFormat__Luk_co_demon_seasip_libdsk_Geometry_2IIB
  (JNIEnv *env, jobject self, jobject jg, jint cyl, jint head, jbyte filler)
{
	DSK_PDRIVER   drv, drv2;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	if (!err) err = dsk_apform(drv, &dg, cyl, head, filler);
	drv2  = driver_from_java(env, self);
	if (!err) err = geom_to_java(env, jg, &dg);
	drv2  = driver_from_java(env, self);
	check_error(env, err);
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    autoFormat
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;IB)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_autoFormat__Luk_co_demon_seasip_libdsk_Geometry_2IB
  (JNIEnv *env, jobject self, jobject jg, jint track, jbyte filler)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);
	if (!err) err = dsk_alform(drv, &dg, track, filler);
	if (!err) err = geom_to_java(env, jg, &dg);
	check_error(env, err);
}
/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    probeGeometry
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_probeGeometry
  (JNIEnv *env, jobject self, jobject jg)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	if (!err) err = dsk_getgeom(drv, &dg);
	if (!err) err = geom_to_java(env, jg, &dg);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    sectorID
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;ILuk/co/demon/seasip/libdsk/FormatData;)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_sectorID__Luk_co_demon_seasip_libdsk_Geometry_2ILuk_co_demon_seasip_libdsk_FormatData_2
  (JNIEnv *env, jobject self, jobject jg, jint track, jobject result)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	DSK_FORMAT buf;	
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	if (!err) err = dsk_lsecid(drv, &dg, track, &buf);
	if (!err) err = format_to_java(env, result, &buf);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    sectorID
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;IILuk/co/demon/seasip/libdsk/FormatData;)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_sectorID__Luk_co_demon_seasip_libdsk_Geometry_2IILuk_co_demon_seasip_libdsk_FormatData_2
  (JNIEnv *env, jobject self, jobject jg, jint cyl, jint sec, jobject result)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	DSK_FORMAT buf;	
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	if (!err) err = dsk_psecid(drv, &dg, cyl, sec, &buf);
	if (!err) err = format_to_java(env, result, &buf);
	check_error(env, err);
}



/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    seek
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;I)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_seek__Luk_co_demon_seasip_libdsk_Geometry_2I
  (JNIEnv *env, jobject self, jobject jg, jint track)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	if (!err) err = dsk_lseek(drv, &dg, track);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    seek
 * Signature: (Luk/co/demon/seasip/libdsk/Geometry;II)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_seek__Luk_co_demon_seasip_libdsk_Geometry_2II
  (JNIEnv *env, jobject self, jobject jg, jint cyl, jint head)
{
	DSK_PDRIVER   drv;
	DSK_GEOMETRY dg;
	dsk_err_t err;
	
	drv   = driver_from_java(env, self);
	err   = geom_from_java  (env, jg, &dg);

	if (!err) err = dsk_pseek(drv, &dg, cyl, head);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    enumDrivers
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uk_co_demon_seasip_libdsk_Drive_enumDrivers
  (JNIEnv *env, jclass clazz, jint index)
{
	char *s;	

	dsk_err_t err = dsk_type_enum(index, &s);

	if (check_error(env, err)) return NULL;
	if (s == NULL) return NULL;
	return (*env)->NewStringUTF(env, s);	
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    getDriverName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uk_co_demon_seasip_libdsk_Drive_getDriverName
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER   drv;
	const char *s;	
	drv   = driver_from_java(env, self);
	s = dsk_drvname(drv);
	if (!s) return NULL;
	return (*env)->NewStringUTF(env, s);	
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    getDriverDesc
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uk_co_demon_seasip_libdsk_Drive_getDriverDesc
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER   drv;
	const char *s;	
	drv   = driver_from_java(env, self);
	s = dsk_drvdesc(drv);
	if (!s) return NULL;
	return (*env)->NewStringUTF(env, s);	
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    getCompressName
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uk_co_demon_seasip_libdsk_Drive_getCompressName
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER   drv;
	const char *s;	
	drv   = driver_from_java(env, self);
	s = dsk_compname(drv);
	if (!s) return NULL;
	return (*env)->NewStringUTF(env, s);	
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    getCompressDesc
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uk_co_demon_seasip_libdsk_Drive_getCompressDesc
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER   drv;
	const char *s;	
	drv   = driver_from_java(env, self);
	s = dsk_compdesc(drv);
	if (!s) return NULL;
	return (*env)->NewStringUTF(env, s);	
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    getComment
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_uk_co_demon_seasip_libdsk_Drive_getComment
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER   drv;
	char *s;	
	dsk_err_t err;

	drv   = driver_from_java(env, self);
	err   = dsk_get_comment(drv, &s);
	if (err || !s) 
	{
		check_error(env, err);
		return NULL;
	}
	return (*env)->NewStringUTF(env, s);	
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    setComment
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_setComment
  (JNIEnv *env, jobject self, jstring str)
{
	const char *cmt = NULL;
	dsk_err_t err;
	DSK_PDRIVER   drv;

	drv   = driver_from_java(env, self);
	if (str != NULL) 
		cmt = (*env)->GetStringUTFChars(env, str, NULL);
	err = dsk_set_comment(drv, cmt);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    getRetry
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_uk_co_demon_seasip_libdsk_Drive_getRetry
  (JNIEnv *env, jobject self)
{
	DSK_PDRIVER   drv;
	unsigned retry;
	dsk_err_t err;

	drv   = driver_from_java(env, self);
	err   = dsk_get_retry(drv, &retry);
	if (err) 
	{
		check_error(env, err);
		return 0;
	}
	return retry;
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Drive
 * Method:    setRetry
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Drive_setRetry
  (JNIEnv *env, jobject self, jint retry)
{
	dsk_err_t err;
	DSK_PDRIVER   drv;

	drv   = driver_from_java(env, self);
	err = dsk_set_retry(drv, retry);
	check_error(env, err);
}


/*************************** The Geometry class *****************************/

/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    ps2ls
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_ps2ls
  (JNIEnv *env, jobject jg, jint cyl, jint head, jint sector)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	dsk_lsect_t lsect;
	
	err   = geom_from_java  (env, jg, &dg);
	if (!err) err = dg_ps2ls(&dg, cyl, head, sector, &lsect);
	check_error(env, err);
	return lsect;
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    ls2ps
 * Signature: (I[I)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_ls2ps
  (JNIEnv *env, jobject jg, jint lsect, jintArray arr)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	dsk_pcyl_t cyl;
	dsk_phead_t head;
	dsk_psect_t sect;
	jint buf[3];
	
	err   = geom_from_java  (env, jg, &dg);
	if (!err) err = dg_ls2ps(&dg, lsect, &cyl, &head, &sect);
	check_error(env, err);
	buf[0] = cyl;
	buf[1] = head;
	buf[2] = sect;
	(*env)->SetIntArrayRegion(env, arr, 0, 3, buf);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    pt2lt
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_pt2lt
  (JNIEnv *env, jobject jg, jint cyl, jint head)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	dsk_ltrack_t ltrack;
	
	err   = geom_from_java  (env, jg, &dg);
	if (!err) err = dg_pt2lt(&dg, cyl, head, &ltrack);
	check_error(env, err);
	return ltrack;
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    dg_lt2pt
 * Signature: (I[I)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_dg_1lt2pt
  (JNIEnv *env, jobject jg, jint ltrack, jintArray arr)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	dsk_pcyl_t cyl;
	dsk_phead_t head;
	jint buf[2];
	
	err   = geom_from_java  (env, jg, &dg);
	if (!err) err = dg_lt2pt(&dg, ltrack, &cyl, &head);
	if (check_error(env, err)) return;
	buf[0] = cyl;
	buf[1] = head;
	(*env)->SetIntArrayRegion(env, arr, 0, 2, buf);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    dosGeometry
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_dosGeometry
  (JNIEnv *env, jobject jg, jbyteArray arr)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dg_dosgeom(&dg, buf);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	if (!err) err = geom_to_java(env, jg, &dg);
	check_error(env, err);
}


/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    apricotGeometry
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_apricotGeometry
  (JNIEnv *env, jobject jg, jbyteArray arr)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dg_aprigeom(&dg, buf);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	if (!err) err = geom_to_java(env, jg, &dg);
	check_error(env, err);
}
/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    pcwGeometry
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_pcwGeometry
  (JNIEnv *env, jobject jg, jbyteArray arr)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dg_pcwgeom(&dg, buf);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	if (!err) err = geom_to_java(env, jg, &dg);
	check_error(env, err);
}

/*
 * Class:     uk_co_demon_seasip_libdsk_Geometry
 * Method:    cpm86Geometry
 * Signature: ([B)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_Geometry_cpm86Geometry
  (JNIEnv *env, jobject jg, jbyteArray arr)
{
	DSK_GEOMETRY dg;
	dsk_err_t err;
	jbyte *buf;
	
	err   = geom_from_java  (env, jg, &dg);

	buf = (*env)->GetByteArrayElements(env, arr, NULL);
	if (!err) err = dg_cpm86geom(&dg, buf);
	(*env)->ReleaseByteArrayElements(env, arr, buf, 0);
	if (!err) err = geom_to_java(env, jg, &dg);
	check_error(env, err);
}


/*************************** The FormatType class *****************************/

/*
 * Class:     uk_co_demon_seasip_libdsk_FormatType
 * Method:    stdFormat
 * Signature: (ILuk/co/demon/seasip/libdsk/Geometry;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_uk_co_demon_seasip_libdsk_FormatType_stdFormat
  (JNIEnv *env, jclass clazz, jint type, jobject jg, jobjectArray descs)
{
	DSK_GEOMETRY dg;
	dsk_cchar_t name, desc;
	dsk_err_t err;
	jstring sname = NULL, sdesc = NULL;

	if (jg != NULL) err   = geom_from_java  (env, jg, &dg);
	else		err   = DSK_ERR_OK;

	if (!err) err = dg_stdformat(&dg, type, &name, &desc);
	if (jg != NULL && (!err)) err = geom_to_java(env, jg, &dg);
	if (check_error(env, err)) return;

	if (descs)
	{	
		if (name) sname = (*env)->NewStringUTF(env, name);
		if (desc) sdesc = (*env)->NewStringUTF(env, desc);
		(*env)->SetObjectArrayElement(env, descs, 0, sname);
		(*env)->SetObjectArrayElement(env, descs, 1, sdesc);
	}
}

JNIEXPORT jstring JNICALL Java_uk_co_demon_seasip_libdsk_LibDsk_version
  (JNIEnv *env, jclass clazz)
{
	return (*env)->NewStringUTF(env, LIBDSK_VERSION);
}




#endif /* def HAVE_JNI_H */
