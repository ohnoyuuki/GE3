#pragma once
class TextureManger
{

public:

	

	//シングルトンインスタンスの取得
	static TextureManger* GetInstance();
	//終了
	void Finalize();

private:
	static TextureManger* instance;
	TextureManger() = default;
	~TextureManger() = default;
	TextureManger(TextureManger&) = delete;
	TextureManger& operator = (TextureManger&) = delete;

};

