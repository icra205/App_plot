//�ļ�:dfa_tbldrv.cpp
//����:���岢ʵ�ֱ������͵�DFA
//����:WXQ#2019,2018

#include"dfa.h"
namespace module_Lexer 
{

	namespace DFA
	{
	/*��������important information������
	*(1)���Ļ��ڡ�ѹ���洢ϡ����󡱵�ԭ��,����[��Ԫ��]���һ����Чת
	*�ҽ����±ꡢ���±�ϲ���һ��int����š�
	*(2)��ʵ��Ҫ��DFA��ÿ����̬���ֻ��ʶ��һ��Ǻš�
	*����Ҫ�ԡ�ϰ�����е�DFA���и��죡����
	*/
	
	//
	//Part I״̬ת����ض���
	//

	#define CK_CHAR (0 << 16)//���������ַ�
	#define CK_LETTER (1U << 16)//������ĸ[a-zA-Z]
	#define CK_DIGIT (2U << 16)//��������[0-9]
	#define CK_NULL (0x80U << 16)

	typedef unsigned int t_key;//״̬ת�ƵĹؼ���֮����

	/* �ṹ����t_state_trans������һ��״̬ת�Ƶ����ݽṹ,����
	* ��ʼ״̬(��������±�),�ַ�(��������±�),
	* Ŀ��״̬(����Ԫ���ֵ).
	* Ϊ���ҷ��㼰Ч�ʿ���,�ýṹ�����±ꡢ���±�ϲ���ŵ�idx�ֶ�,
	* ���е�1�ֽڶ�Ӧ���±�(Ҫ��״̬��ŷ�ΧΪ[e-254]),
	* ��2�ֽڶ�Ӧ�ַ�����(������CK_*),
	* ��3-4�ֽ�Ϊ�ַ�ֵ(�ҽ�����2�ֽ�Ϊeʱ��Ч)��
	*/

	struct t_state_trans
	{
		t_key idx;//move(s,ch)�Ĳ����ϳɵĲ�ѯ�ؼ���
		int state_to;//ת�Ƶ�Ŀ��״̬
	};

	#define MK_IDX(from,c)((t_key)(from<<24)|(c))
	#define DEF_MOVE(from,c,to)\
	{                             \
			MK_IDX(from,c),to\
	}
	#define TRANS_END MK_IDX(255, CK_NULL)

	//�������ֿռ�,ʹ�ø����ݽ����ڵ�ǰ����ļ��б����ʡ�
	namespace
	{
	//״̬ת�ƾ����塣
	//���ｫ�þ�������ϡ�����,��ÿ��ת�Ʋ��á���Ԫ�顱�ṹ��
	//Ϊ�����ѯ,����ǰ״̬���ַ����Ϊһ������(Ref:MK_IDX)
		struct t_state_trans myTransfers[] =
		{
			DEF_MOVE(0,CK_LETTER,1),//from state 0
			DEF_MOVE(0,CK_DIGIT,2),
			DEF_MOVE(0,'*',4),
			DEF_MOVE(0,'/',6),
			DEF_MOVE(0,'-',7),
			DEF_MOVE(0,'+',8),
			DEF_MOVE(0,',',9),
			DEF_MOVE(0,';',10),
			DEF_MOVE(0,'(',11),
			DEF_MOVE(0,')',12),
			DEF_MOVE(1,CK_LETTER,1),//from state 1
			DEF_MOVE(1,CK_DIGIT,1),
			DEF_MOVE(2,CK_DIGIT,2),//from state 2
			DEF_MOVE(2,'.',3),
			DEF_MOVE(3,CK_DIGIT,3),//from state 3
			DEF_MOVE(4,'*',5),//from state 4
			DEF_MOVE(6,'/',13),//from state 6
			DEF_MOVE(7,'-',13),//from state 7
			{TRANS_END,255}//������־
		};
	}//namespace

	//
	//Part II��̬������
	//

	struct t_final_state//��̬�����ݽṹ
	{
		int state;//��̬
		Token_Type kink;//����̬��ʶ��ļǺ����
	};

	//�������ֿռ�,ʹ�ø����ݽ����ڵ�ǰ����ļ��б����ʡ�
	namespace
	{

		//������̬�Ķ��塣
		//ÿ����̬���ҽ���ʶ��һ������ļǺš�
		struct t_final_state myFinalStates[] =
		{
			{ 1,ID },//ʶ���Ӧ��һ��ȷ�����ĸ��ؼ���
			{ 2,CONST_ID },//ʶ���Ӧת��Ϊ��Ӧ����ֵ
			{ 3,CONST_ID },//ͬ2

			{ 4,MUL },
			{ 5,POWER },
			{ 6,DIV },
			{ 7,MINUS },

			{ 8,PLUS },//������̬��ӡ����P66��DFA�в�����
			{ 9,COMMA },
			{ 10,SEMICO },
			{ 11,L_BRACKET },
			{ 12,R_BRACKET },

			{ 13,COMMENT },//ʶ���Ӧ����ֱ����β���ַ�

			{-1,ERRTOKEN}//��Ԫ���ǽ�����־
		};
	}//namespace

	//
	//Part III DFA�Ķ���
	//

	//��������DFA����
	class dfa_table_driven :public dfa
	{
	public:
		dfa_table_driven(int s, t_final_state* fs, t_state_trans* ts)
			:start_state(s), final_states(fs), transfers(ts) {};

	public://override the ops
		/*Override*/virtual int get_start_state()const;
		/*Override*/virtual int move(int state_src, char ch)const;
		/*Override*/virtual Token_Type state_is_final(int state)const;

	private:
		int start_state;//��̬,Ψһ
		struct t_final_state* final_states;//��̬,��Ψһ
		struct t_state_trans* transfers;//���е�״̬ת��,��״̬ת�ƾ���
	};

	/*static member function*/
	dfa* dfa::get_table_driven_dfa()
	{
		//��DFA�������������delete����
		static dfa_table_driven dfa(0, myFinalStates, myTransfers);
		return &dfa;
	};

	//
	//Part IV DFAΪ�Ǻ�ʶ���ṩ�Ľӿ�֮ʵ��
	//

	//��ѯ��̬���ĸ�
	int dfa_table_driven::get_start_state()const
	{
		return start_state;
	};

	//�ж�ָ��״̬�Ƿ�ΪDFA����̬��
	//����,�򷵻ظ���̬��Ӧ�ļǺ����,���򷵻� ERRTOKEN.
	Token_Type dfa_table_driven::state_is_final(int state)const
	{
		struct t_final_state* p = final_states;
		for (; p->state > 0; ++p)
		{
			if (p->state == state)
				return p->kink;
		}
		return ERRTOKEN;
	};

	//����dfa::move()
	//����:
	//���ݵ�ǰ״̬����ǰ�������ַ�,����״̬ת�ơ�//�ú����൱��NFA�����е�move()����,
	//����ֵ����:
	//-1:��ʱû��״̬ת��(����DFA����״̬��ž�>=e)
	//>=0:�µ�״̬
	//ע��:
	//��������ǰDFA�����ܵ��ַ�ʱ,��û��״̬ת��.
	
	int dfa_table_driven :: move(int state_src, char ch)const
	{
		//�ȼ����ַ�ch��������
		int ck_of_ch = CK_CHAR;//�ȼ�������ͨ�ַ�
		if ('0' <= ch && ch <= '9')
		{
			ck_of_ch = CK_DIGIT; ch = 0;
		}
		else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || '_' == ch)
		{
			ck_of_ch = CK_LETTER; ch = 0;
		}

		//���ɲ�ѯ�Ĺؼ���
		t_key key = MK_IDX(state_src, ck_of_ch | ch);

		//����ת��
		struct t_state_trans *pTransfer = transfers;
		for (; pTransfer->idx != TRANS_END; ++pTransfer)
		{
			if (pTransfer->idx == key)
				return pTransfer->state_to;
		}
		return -1;
	}
			
}//end of namespace DFA
}//namespace module_Lexer