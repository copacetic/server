#ifndef _CLIENT
#define _CLIENT

#define BUF_LENGTH 100

#include <cstring>

struct batch
{
	bool firstByte;
	int counter, length;
	unsigned char buf[BUF_LENGTH];
};

class Client
{
public:
	Client();
	//stat methods
	inline void set_x(float _x){x = _x;};
   	inline void set_y(float _y){y = _y;};
    inline void set_z(float _z){z = _z;};
    inline void set_r(float _r){r = _r;};
	inline void set_faction(int _f){faction = _f;};
	inline void set_money(int _m){money = _m;};
	inline void set_status(int _s){status = _s;};
	inline void set_pkp(int _p){pkp = _p;};
	// Set Skills
	inline void set_axe(float _a){axe = _a;};
	inline void set_dagger(float _d){dagger = _d;};
	inline void set_mace(float _m){mace = _m;};
	inline void set_shield(float _s){shield = _s;};
	inline void set_staff(float _s){staff = _s;};
	inline void set_sword(float _s){shield = _s;};
	inline void set_bow(float _b){bow = _b;};
	inline void set_crossbow(float _c){crossbow = _c;};
	inline void set_gun(float _g){gun = _g;};
	inline void set_heavy(float _h){heavy = _h;};
	inline void set_medium(float _m){medium = _m;};
	inline void set_light(float _l){light = _l;};
	inline void set_heal(float _h){heal - _h;};
	inline void set_protection(float _p){protection = _p;};
	inline void set_buff(float _b){buff = _b;};
	// Set Armor
	inline void set_lhand(int _l){lhand = _l;};
	inline void set_rhand(int _r){rhand = _r;};
	inline void set_head(int _h){rhand = _h;};
	inline void set_shoulder(int _s){shoulder = _s;};
	inline void set_chest(int _c){chest = _c;};
	inline void set_gloves(int _g){gloves = _g;};
	inline void set_pants(int _p){pants = _p;};
	inline void set_boots(int _b){boots = _b;};
	
	void set_location(float, float, float, float);
	inline void set_health(int _health){health = _health;};
	void set_name(char _name[]);
	inline void set_id(int _id){id = _id;};
	inline int get_id(){return id;};
	//batch methods
	void reset_batch();
	inline void set_b_first(bool state){b.firstByte = state;};
	inline void set_b_length(int _length){b.length = _length;};
	inline void inc_b_counter(int inc){b.counter += inc;};
	inline bool get_b_first(){return b.firstByte;};
	inline int* get_b_length(){return &b.length;};
	inline int* get_b_counter(){return &b.counter;};
	inline unsigned char * get_b_buf(){return b.buf;};

	// Get Methods
	std::string get_name(){return name;};
	int get_x(){return x;};
	int get_y(){return y;};
	int get_z(){return z;};
	int get_r(){return r;};
	int get_money(){return money;};
	int get_pkp(){return pkp;};
	// int get_status(){return status;}; // Not necessary?
	float get_axe(){return axe;};
	float get_dagger(){return dagger;};
	float get_mace(){return mace;};
	float get_shield(){return shield;};
	float get_staff(){return staff;};
	float get_sword(){return sword;};
	float get_bow(){return bow;};
	float get_crossbow(){return crossbow;};
	float get_gun(){return gun;};
	float get_heavy(){return heavy;};
	float get_medium(){return medium;};
	float get_light(){return light;};
	float get_heal(){return heal;};
	float get_protection(){return protection;}; // Lol
	float get_buff(){return buff;}; // If only it was that easy
	int get_lhand(){return lhand;};
	int get_rhand(){return rhand;};
	int get_head(){return head;};
	int get_shoulder(){return shoulder;};
	int get_chest(){return chest;};
	int get_gloves(){return gloves;};
	int get_pants(){return pants;};
	int get_boots(){return boots;};
private:

	// User Attributes
	float x, y, z, r;
	char* name;
	unsigned int id;
	int health;
	batch b;
	unsigned int faction;
	unsigned int money;
	unsigned int status;
	unsigned int pkp;
	// Skills
	float axe;
	float dagger;
	float mace;
	float shield;
	float staff;
	float sword;
	float bow;
	float crossbow;
	float gun;
	float heavy;
	float medium;
	float light;
	float heal;
	float protection;
	float buff;
	// Armor
	int lhand;
	int rhand;
	int head;
	int shoulder;
	int chest;
	int gloves;
	int pants;
	int boots;
};

#endif
