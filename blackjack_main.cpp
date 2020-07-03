#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

struct Player {
	int budget;
	int win;
	int draw;
};


// ī�� ���� �ִ� �Լ�
void Shuffle(vector<int> *card_deck)
{
	if(!card_deck->empty())	//���� ������� �ʴٸ�
		card_deck->clear();	//���� ī�带 ��� �����Ѵ�.

	for (int i = 0; i < 24; i++) {
		card_deck->push_back(1);	//A
		card_deck->push_back(2);
		card_deck->push_back(3);
		card_deck->push_back(4);
		card_deck->push_back(5);
		card_deck->push_back(6);
		card_deck->push_back(7);
		card_deck->push_back(8);
		card_deck->push_back(9);
		card_deck->push_back(10);
		card_deck->push_back(11);	//J
		card_deck->push_back(12);	//Q
		card_deck->push_back(13);	//K
	}
	std::random_shuffle(card_deck->begin(), card_deck->end());
}


// ī�� ���� ���� ������ ����ϴ� �Լ�
int SumofNum(int hand[30]) 
{
	int sum = 0;
	int a_count = 0;
	int a_sum = 0;
	bool bust_flag = true;
	for (int i = 0; i < 30; i++) {
		if (hand[i] == 0) {
			break;
		}
		else if (hand[i] == 1) {	//A�� ���
			a_count++;	//A�� ������ ������
		}
		else if (hand[i] >= 10) {	//10, J, Q, K
			sum += 10;
		}
		else {		// 2 ~ 9
			sum += hand[i];
		}
	}
	// A�� 1�� ������� 11�� ������� �Ǵ� - Greedy Algorithm from biggest
	for (int i = 0; i < a_count + 1; i++) {	// A�� ����(p, 1-p) ������ŭ �ݺ�
		for (int j = 0; j < a_count - i; j++) {	// 11�� ���ϴ� Ƚ��
			a_sum += 11;
		}
		for (int j = 0; j < i; j++) {	// 1�� ���ϴ� Ƚ��
			a_sum += 1;
		}
		if (sum + a_sum > 21) {	// ���� �հ� a�� ���� 21���� ũ�ٸ�
			a_sum = 0;	//bust�̱� ������ a_sum�� �ٽ� �����ش�.
		}
		else {
			bust_flag = false; //bust���� ���� ���
			sum += a_sum;	//����� �����ְ�
			break;	//��
		}
	}
	if (bust_flag == true) {	//a�� ��� 1�� �ص� bust�� ��� 
		for (int i = 0; i < a_count; i++) {
			sum += 1;	//�׳� 1�� �� �����ְ� ������.
		}
	}
	return sum;	//ī���� ���� ��ȯ�Ѵ�.
}


// ī�� ī���� �ϴ� �Լ�
double CardCounting(int card_count[14])
{
	int total_used = 0;
	int total = 312;	// A ~ K
	int over_ten_used = 0;
	int over_ten = 120;	// A, 10, J, Q, K
	int two_to_six_used = 0;
	int two_to_six = 120;
	double percentage = 0.0;
	for (int i = 1; i < 14; i++) {
		total_used += card_count[i];	//��ü ����� ī�� ����
	}
	for (int i = 10; i < 14; i++) {	//10�� ������ A�� ī�����Ѵ�.
		over_ten_used += card_count[i];	//10, J, Q, K ����� ����
	}
	over_ten_used += card_count[1];	//A����
	for (int i = 2; i < 7; i++) {
		two_to_six_used += card_count[i];	// 2, 3, 4, 5, 6 ����� ����
	}
	percentage = ((double)two_to_six_used - (double)over_ten_used) / ((double)total - (double)total_used);
	return percentage;
}


// ���� �ݾ� ���� �Լ�
int Betting(int card_count[14], bool counting_flag)
{
	int alpha = 1;
	double k = 0.0;
	if (counting_flag) {	//ī���� �÷��̾�� Ȯ���� ���� �����Ѵ�.
		k = (1 + CardCounting(card_count)) * alpha;
		return (int)(100 * k);
	}
	else {	//simple player�� 100���� �����Ѵ�.
		return 100;
	}
}


// Hit���� Stand���� �����ϴ� �Լ�
bool HitStatus(int hand[30], int card_count[14], int second_card_of_dealer, bool counting_flag)
{
	double overten_percentage = 0.0;
	if (counting_flag) {	//card counting player�� ���
		//ī�� ī���� �˰��� ���
		overten_percentage = CardCounting(card_count);
		if (SumofNum(hand) >= 17) {
			return true;	//17���� ũ�ٸ� stand
		}
		else {	// ���� �� �а� 17���� �۾Ƶ�
			if (SumofNum(hand) <= 11) {	//11���� ������ ������ �̾Ƶ� bust���� �ʱ⿡
				return false;	//hit�Ѵ�.
			}
			if (2 <= second_card_of_dealer && second_card_of_dealer <= 6) {	//������ ������ �а� 6���϶��
				// ������ A�� �����ϰ� ������ hit�� �ؾ� �Ѵ�. 
				// �̶� 10 �̻��� ī�尡 ���� Ȯ���� 50%�� �Ѵ´ٸ� 
				// ������ ������ bust�� Ȯ���� �����Ƿ� player�� stand�ϰ� �ȴ�.
				if (overten_percentage > 0) {
					return true;	//stand�� �Ͽ� ������ bust�� ����غ���.
				}
			}
			return false;
		}
	}
	else {	//simple player & dealer �� ���
		if (SumofNum(hand) >= 17) {
			return true;	//stand ��ȯ
		}
		else return false;	//hit ��ȯ
	}
}


// ���� ���� �Լ�
void PlayGame(int game_count, struct Player *simple_p, struct Player *counting_p)
{
	int card_count[14] = { 0, };
	int temp, temp_dealer, temp_simple, temp_counting, first_card_of_dealer, second_card_of_dealer;
	int dealer_hand[30] = { 0, };
	int simple_hand[30] = { 0, };
	int counting_hand[30] = { 0, };
	int simple_betting;
	int counting_betting;
	bool simple_bust = false, counting_bust = false;
	vector<int> card_deck;
	//�Է��� Ƚ����ŭ ���� ����
	for (int j = 0; j < game_count; j++) {
		// Step 1. ������ ī�尡 80% �̻� ���Ǿ����� �Ǵ�
		// Step 1.1 80% �̻��̶�� Shuffle (card counting data clear)
		if (card_deck.size() < 63) {		//80%�̻� ���Ǿ��ٸ�
			for (int i = 0; i < 14; i++) {
				card_count[i] = 0;	//ī���� �ʱ�ȭ
			}
			Shuffle(&card_deck);	//Shuffle
		}

		// ���� ,�÷��̾� �� �ʱ�ȭ
		for (int i = 0; i < 30; i++) {
			simple_hand[i] = 0;
			counting_hand[i] = 0;
			dealer_hand[i] = 0;
		}
		// �÷��̾� ���� �ݾ� ����
		simple_betting = Betting(card_count, false);
		counting_betting = Betting(card_count, true);

		// Step 1.2 80% �̸��̶�� ������ �ʹ� �и� 2�徿 ������ ��������
		temp = card_deck.back();
		card_deck.pop_back();
		dealer_hand[0] = temp;	//���� ����
		// �÷��̾ ������ ù��° ī��� �� �� ����. ����
		// ���Ŀ� ī���� ���ֱ� ���� �ӽ� ����
		first_card_of_dealer = temp;	//������ ù��° �д� ī���� ���� �ʴ´�.

		temp = card_deck.back();
		card_deck.pop_back();
		counting_hand[0] = temp;	//ī���� �÷��̾� ����
		card_count[temp]++;	//�ش� ī�� ī��Ʈ

		temp = card_deck.back();
		card_deck.pop_back();
		simple_hand[0] = temp;	//���� �÷��̾� ����
		card_count[temp]++;	//�ش� ī�� ī��Ʈ

		temp = card_deck.back();
		card_deck.pop_back();
		dealer_hand[1] = temp;	//���� ���� = �� �� ��
		second_card_of_dealer = temp;
		card_count[temp]++;	//�ش� ī�� ī��Ʈ

		temp = card_deck.back();
		card_deck.pop_back();
		counting_hand[1] = temp;	//ī���� �÷��̾� ����
		card_count[temp]++;	//�ش� ī�� ī��Ʈ

		temp = card_deck.back();
		card_deck.pop_back();
		simple_hand[1] = temp;	//���� �÷��̾� ����
		card_count[temp]++;	//�ش� ī�� ī��Ʈ

		// Step 2. ������ �÷��̾ ������ ��ĥ �� ���� ��ٸ�
		// Step 1) Card Counting Player ���� ���� -> ī�� ī���� �Ͽ� ����
		for (int i = 2; i < 30; i++) {
			// Step 2) ������ ī�尡 17���� ���ų� ũ�ٸ�? stand -> �÷��̾� ���� ����
			if (HitStatus(counting_hand, card_count, second_card_of_dealer, true) == false) { // �۴ٸ� hit
				temp = card_deck.back();
				card_deck.pop_back();
				counting_hand[i] = temp;	//�÷��̾� ���� �� ����
				card_count[temp]++;	//�ش� ī�� ī��Ʈ
			}
			else break;	//stand
		}
		card_count[first_card_of_dealer]++; //������ ù��° �и� ī���� ��
		first_card_of_dealer = 0;

		// Step 2) Simple Player ���� ���� -> ���� ��Ģ��� ����
		for (int i = 2; i < 30; i++) {
			// Step 2) ������ ī�尡 17���� ���ų� ũ�ٸ�? stand -> �÷��̾� ���� ����
			if (HitStatus(simple_hand, card_count, second_card_of_dealer, false) == false) { // �۴ٸ� hit
				temp = card_deck.back();
				card_deck.pop_back();
				simple_hand[i] = temp;	//�÷��̾� ���� �� ����
				card_count[temp]++;	//�ش� ī�� ī��Ʈ
			}
			else break;	//stand
		}

		// Step 2.1 �÷��̾��� ī�尡 21���� �۴ٸ�?
		// Step 2.2 ������ ī�尡 17���� ũ�ų� ���ٸ�?? Stand -> Step 3 ���� �̵�
		// Step 2.3 ������ ī�尡 17���� �۴ٸ� Hit -> Step 2.3�� �̵�.
		for (int i = 2; i < 30; i++) {
			if (HitStatus(dealer_hand, card_count, second_card_of_dealer, false) == false) { // �۴ٸ� hit
				temp = card_deck.back();
				card_deck.pop_back();
				dealer_hand[i] = temp;	//���� ���� �� ����
				card_count[temp]++;	//�ش� ī�� ī��Ʈ
			}
			else break; //stand
		}

		// Step 2.4 �÷��̾� ī�尡 21���� ũ�ٸ�? ������ ������ ���� ��
		if (SumofNum(counting_hand) > 21) {
			counting_p->budget -= counting_betting;	// ���ñݾ׸�ŭ �Ҵ´�,
			counting_bust = true;
		}
		if (SumofNum(simple_hand) > 21) {
			simple_p->budget -= simple_betting;
			simple_bust = true;
		}

		// Step 3. Dealer > Player
		temp_dealer = SumofNum(dealer_hand);
		temp_simple = SumofNum(simple_hand);
		temp_counting = SumofNum(counting_hand);
		// Step 3.1 ������ ũ�ٸ� ������, �÷��̾ ũ�ٸ� �÷��̾� ��, ���ٸ� ���º� -> ����
		// ī���� �÷��̾� ����
		if (counting_bust == false) {	//ī���� �÷��̾ bust���� �ʾ��� ���
			if (temp_dealer > temp_counting) {
				if (temp_dealer > 21) {
					// dealer bust, player win
					counting_p->budget += counting_betting;	// ���ñݾ׸�ŭ ��´�.
					counting_p->win += 1;	//win count
				}
				else {	//player�� �����Ƿ�
					counting_p->budget -= counting_betting;	// ���ñݾ׸�ŭ �Ҵ´�,
					//dealer win, player lose
				}
			}
			else if (temp_dealer == temp_counting) {	//if draw
				counting_p->draw += 1;	//draw count
			}
			else {	//player win
				counting_p->budget += counting_betting;	// ���ñݾ׸�ŭ ��´�.
				counting_p->win += 1;
			}
		}
		if (simple_bust == false) {	//���� �÷��̾ bust���� �ʾҴٸ�
			if (temp_dealer > temp_simple) {
				if (temp_dealer > 21) {
					// dealer bust, player win
					simple_p->budget += simple_betting;	// ���ñݾ׸�ŭ ��´�.
					simple_p->win += 1;	//win count
				}
				else {	//player�� �����Ƿ�
					simple_p->budget -= simple_betting;	// ���ñݾ׸�ŭ �Ҵ´�,
					//dealer win, player lose
				}
			}
			else if (temp_dealer == temp_simple) {	//if draw
				simple_p->draw += 1;	//draw count
			}
			else {	//player win
				simple_p->budget += simple_betting;	// ���ñݾ׸�ŭ ��´�.
				simple_p->win += 1;
			}
		}
		counting_bust = false;
		simple_bust = false;
	}
}


int main() 
{
	// Step 0. blackjack �ʱ� ���� ����
	int game_count = 0;		//���� Ƚ��
	cout << "Play Game : ";
	cin >> game_count;		//���� Ƚ�� �Է¹���
	srand((unsigned int)time(NULL));	//random generate�� ����

	struct Player player_card_counting;
	player_card_counting.budget = 100000;	//player�� ���� 10�������� �ʱ��ں� �ο�
	player_card_counting.win = 0;		//�¸� Ƚ��
	player_card_counting.draw = 0;	//��� Ƚ��

	struct Player player_simple;	//player����
	player_simple.budget = 100000;	//player�� ���� 10�������� �ʱ��ں� �ο�
	player_simple.win = 0;		//�¸� Ƚ��
	player_simple.draw = 0;	//��� Ƚ��

	PlayGame(game_count, &player_simple, &player_card_counting);	//ī�� ī���� �÷��̾�
	//PlayGame(game_count, &player_simple, false);	//���� �÷��̾�
	
	// ��� ���
	cout << "Card counting Player win : " << player_card_counting.win
		<< " / draw : " << player_card_counting.draw
		<< " / �·� : " << 100 * ((double)player_card_counting.win / (double)(game_count - player_card_counting.draw))
		<< "%/ ���� �� : " << player_card_counting.budget << endl;

	cout << "Simple Player win : " << player_simple.win
		<< " / draw : " << player_simple.draw
		<< " / �·� : " << 100 * ((double)player_simple.win / (double)(game_count - player_simple.draw))
		<< "%/ ���� �� : " << player_simple.budget << endl;

	return 0;
}