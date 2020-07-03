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


// 카드 섞어 주는 함수
void Shuffle(vector<int> *card_deck)
{
	if(!card_deck->empty())	//덱이 비어있지 않다면
		card_deck->clear();	//남은 카드를 모두 삭제한다.

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


// 카드 숫자 합이 얼마인지 계산하는 함수
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
		else if (hand[i] == 1) {	//A인 경우
			a_count++;	//A의 개수를 세어줌
		}
		else if (hand[i] >= 10) {	//10, J, Q, K
			sum += 10;
		}
		else {		// 2 ~ 9
			sum += hand[i];
		}
	}
	// A를 1로 사용할지 11로 사용할지 판단 - Greedy Algorithm from biggest
	for (int i = 0; i < a_count + 1; i++) {	// A의 조합(p, 1-p) 개수만큼 반복
		for (int j = 0; j < a_count - i; j++) {	// 11을 더하는 횟수
			a_sum += 11;
		}
		for (int j = 0; j < i; j++) {	// 1을 더하는 횟수
			a_sum += 1;
		}
		if (sum + a_sum > 21) {	// 기존 합과 a의 합이 21보다 크다면
			a_sum = 0;	//bust이기 때문에 a_sum을 다시 구해준다.
		}
		else {
			bust_flag = false; //bust하지 않은 경우
			sum += a_sum;	//결과를 더해주고
			break;	//끝
		}
	}
	if (bust_flag == true) {	//a를 모두 1로 해도 bust인 경우 
		for (int i = 0; i < a_count; i++) {
			sum += 1;	//그냥 1을 다 더해주고 끝낸다.
		}
	}
	return sum;	//카드의 합을 반환한다.
}


// 카드 카운팅 하는 함수
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
		total_used += card_count[i];	//전체 사용한 카드 개수
	}
	for (int i = 10; i < 14; i++) {	//10에 근접한 A도 카운팅한다.
		over_ten_used += card_count[i];	//10, J, Q, K 사용한 개수
	}
	over_ten_used += card_count[1];	//A개수
	for (int i = 2; i < 7; i++) {
		two_to_six_used += card_count[i];	// 2, 3, 4, 5, 6 사용한 개수
	}
	percentage = ((double)two_to_six_used - (double)over_ten_used) / ((double)total - (double)total_used);
	return percentage;
}


// 베팅 금액 설정 함수
int Betting(int card_count[14], bool counting_flag)
{
	int alpha = 1;
	double k = 0.0;
	if (counting_flag) {	//카운팅 플레이어는 확률에 따라 베팅한다.
		k = (1 + CardCounting(card_count)) * alpha;
		return (int)(100 * k);
	}
	else {	//simple player는 100원씩 베팅한다.
		return 100;
	}
}


// Hit할지 Stand할지 선택하는 함수
bool HitStatus(int hand[30], int card_count[14], int second_card_of_dealer, bool counting_flag)
{
	double overten_percentage = 0.0;
	if (counting_flag) {	//card counting player의 경우
		//카드 카운팅 알고리즘 사용
		overten_percentage = CardCounting(card_count);
		if (SumofNum(hand) >= 17) {
			return true;	//17보다 크다면 stand
		}
		else {	// 지금 내 패가 17보다 작아도
			if (SumofNum(hand) <= 11) {	//11보다 작으면 무엇을 뽑아도 bust하지 않기에
				return false;	//hit한다.
			}
			if (2 <= second_card_of_dealer && second_card_of_dealer <= 6) {	//딜러의 공개된 패가 6이하라면
				// 딜러는 A를 제외하고 무조건 hit을 해야 한다. 
				// 이때 10 이상의 카드가 나올 확률이 50%가 넘는다면 
				// 딜러가 스스로 bust할 확률이 높으므로 player는 stand하게 된다.
				if (overten_percentage > 0) {
					return true;	//stand를 하여 딜러의 bust를 기대해본다.
				}
			}
			return false;
		}
	}
	else {	//simple player & dealer 의 경우
		if (SumofNum(hand) >= 17) {
			return true;	//stand 반환
		}
		else return false;	//hit 반환
	}
}


// 블랙잭 메인 함수
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
	//입력한 횟수만큼 게임 진행
	for (int j = 0; j < game_count; j++) {
		// Step 1. 딜러가 카드가 80% 이상 사용되었는지 판단
		// Step 1.1 80% 이상이라면 Shuffle (card counting data clear)
		if (card_deck.size() < 63) {		//80%이상 사용되었다면
			for (int i = 0; i < 14; i++) {
				card_count[i] = 0;	//카운팅 초기화
			}
			Shuffle(&card_deck);	//Shuffle
		}

		// 딜러 ,플레이어 패 초기화
		for (int i = 0; i < 30; i++) {
			simple_hand[i] = 0;
			counting_hand[i] = 0;
			dealer_hand[i] = 0;
		}
		// 플레이어 베팅 금액 설정
		simple_betting = Betting(card_count, false);
		counting_betting = Betting(card_count, true);

		// Step 1.2 80% 미만이라면 딜러가 초반 패를 2장씩 번갈아 나누어줌
		temp = card_deck.back();
		card_deck.pop_back();
		dealer_hand[0] = temp;	//딜러 한장
		// 플레이어가 딜러의 첫번째 카드는 볼 수 없다. 따라서
		// 추후에 카운팅 해주기 위해 임시 저장
		first_card_of_dealer = temp;	//딜러의 첫번째 패는 카운팅 하지 않는다.

		temp = card_deck.back();
		card_deck.pop_back();
		counting_hand[0] = temp;	//카운팅 플레이어 한장
		card_count[temp]++;	//해당 카드 카운트

		temp = card_deck.back();
		card_deck.pop_back();
		simple_hand[0] = temp;	//심플 플레이어 한장
		card_count[temp]++;	//해당 카드 카운트

		temp = card_deck.back();
		card_deck.pop_back();
		dealer_hand[1] = temp;	//딜러 한장 = 총 두 장
		second_card_of_dealer = temp;
		card_count[temp]++;	//해당 카드 카운트

		temp = card_deck.back();
		card_deck.pop_back();
		counting_hand[1] = temp;	//카운팅 플레이어 한장
		card_count[temp]++;	//해당 카드 카운트

		temp = card_deck.back();
		card_deck.pop_back();
		simple_hand[1] = temp;	//심플 플레이어 한장
		card_count[temp]++;	//해당 카드 카운트

		// Step 2. 딜러는 플레이어가 게임을 마칠 때 까지 기다림
		// Step 1) Card Counting Player 게임 시작 -> 카드 카운팅 하여 베팅
		for (int i = 2; i < 30; i++) {
			// Step 2) 두장의 카드가 17보다 같거나 크다면? stand -> 플레이어 게임 종료
			if (HitStatus(counting_hand, card_count, second_card_of_dealer, true) == false) { // 작다면 hit
				temp = card_deck.back();
				card_deck.pop_back();
				counting_hand[i] = temp;	//플레이어 한장 더 받음
				card_count[temp]++;	//해당 카드 카운트
			}
			else break;	//stand
		}
		card_count[first_card_of_dealer]++; //딜러의 첫번째 패를 카운팅 함
		first_card_of_dealer = 0;

		// Step 2) Simple Player 게임 시작 -> 딜러 규칙대로 진행
		for (int i = 2; i < 30; i++) {
			// Step 2) 두장의 카드가 17보다 같거나 크다면? stand -> 플레이어 게임 종료
			if (HitStatus(simple_hand, card_count, second_card_of_dealer, false) == false) { // 작다면 hit
				temp = card_deck.back();
				card_deck.pop_back();
				simple_hand[i] = temp;	//플레이어 한장 더 받음
				card_count[temp]++;	//해당 카드 카운트
			}
			else break;	//stand
		}

		// Step 2.1 플레이어의 카드가 21보다 작다면?
		// Step 2.2 딜러의 카드가 17보다 크거나 같다면?? Stand -> Step 3 으로 이동
		// Step 2.3 딜러의 카드가 17보다 작다면 Hit -> Step 2.3로 이동.
		for (int i = 2; i < 30; i++) {
			if (HitStatus(dealer_hand, card_count, second_card_of_dealer, false) == false) { // 작다면 hit
				temp = card_deck.back();
				card_deck.pop_back();
				dealer_hand[i] = temp;	//딜러 한장 더 받음
				card_count[temp]++;	//해당 카드 카운트
			}
			else break; //stand
		}

		// Step 2.4 플레이어 카드가 21보다 크다면? 딜러의 승으로 게임 끝
		if (SumofNum(counting_hand) > 21) {
			counting_p->budget -= counting_betting;	// 베팅금액만큼 잃는다,
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
		// Step 3.1 딜러가 크다면 딜러승, 플레이어가 크다면 플레이어 승, 같다면 무승부 -> 종료
		// 카운팅 플레이어 먼저
		if (counting_bust == false) {	//카운팅 플레이어가 bust하지 않았을 경우
			if (temp_dealer > temp_counting) {
				if (temp_dealer > 21) {
					// dealer bust, player win
					counting_p->budget += counting_betting;	// 베팅금액만큼 얻는다.
					counting_p->win += 1;	//win count
				}
				else {	//player가 졌으므로
					counting_p->budget -= counting_betting;	// 베팅금액만큼 잃는다,
					//dealer win, player lose
				}
			}
			else if (temp_dealer == temp_counting) {	//if draw
				counting_p->draw += 1;	//draw count
			}
			else {	//player win
				counting_p->budget += counting_betting;	// 베팅금액만큼 얻는다.
				counting_p->win += 1;
			}
		}
		if (simple_bust == false) {	//심플 플레이어가 bust하지 않았다면
			if (temp_dealer > temp_simple) {
				if (temp_dealer > 21) {
					// dealer bust, player win
					simple_p->budget += simple_betting;	// 베팅금액만큼 얻는다.
					simple_p->win += 1;	//win count
				}
				else {	//player가 졌으므로
					simple_p->budget -= simple_betting;	// 베팅금액만큼 잃는다,
					//dealer win, player lose
				}
			}
			else if (temp_dealer == temp_simple) {	//if draw
				simple_p->draw += 1;	//draw count
			}
			else {	//player win
				simple_p->budget += simple_betting;	// 베팅금액만큼 얻는다.
				simple_p->win += 1;
			}
		}
		counting_bust = false;
		simple_bust = false;
	}
}


int main() 
{
	// Step 0. blackjack 초기 상태 설정
	int game_count = 0;		//게임 횟수
	cout << "Play Game : ";
	cin >> game_count;		//게임 횟수 입력받음
	srand((unsigned int)time(NULL));	//random generate를 위해

	struct Player player_card_counting;
	player_card_counting.budget = 100000;	//player의 예산 10만원으로 초기자본 부여
	player_card_counting.win = 0;		//승리 횟수
	player_card_counting.draw = 0;	//비긴 횟수

	struct Player player_simple;	//player설정
	player_simple.budget = 100000;	//player의 예산 10만원으로 초기자본 부여
	player_simple.win = 0;		//승리 횟수
	player_simple.draw = 0;	//비긴 횟수

	PlayGame(game_count, &player_simple, &player_card_counting);	//카드 카운팅 플레이어
	//PlayGame(game_count, &player_simple, false);	//심플 플레이어
	
	// 결과 출력
	cout << "Card counting Player win : " << player_card_counting.win
		<< " / draw : " << player_card_counting.draw
		<< " / 승률 : " << 100 * ((double)player_card_counting.win / (double)(game_count - player_card_counting.draw))
		<< "%/ 남은 돈 : " << player_card_counting.budget << endl;

	cout << "Simple Player win : " << player_simple.win
		<< " / draw : " << player_simple.draw
		<< " / 승률 : " << 100 * ((double)player_simple.win / (double)(game_count - player_simple.draw))
		<< "%/ 남은 돈 : " << player_simple.budget << endl;

	return 0;
}