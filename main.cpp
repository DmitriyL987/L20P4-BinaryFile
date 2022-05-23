#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <random>

void load (){
  std::ofstream file("DeviceStatus.bin", std::ios::binary );
  if (!file.is_open()){
    std::cerr << "Ошибка открытия файла из процедуры load ." << std::endl;
    return;
  }
  int s = 1000;
  int banknotes[6];
  for (int i = 0; i < 6; i++){
  srand(time(nullptr));
  banknotes[i] = rand() % (500 / (i + 1) );
  s -= banknotes[i]; 
  }
  if (s > 0) banknotes[0] += s;
  file.seekp(0);
  file.write((char*)&banknotes, sizeof(int) * 6); 
  file.close();
  std::cout << "Инкаcсация произведена успешно." << std::endl;
}

bool countSumm(int summa, int banknotes[], int curr[], int nominal[], int S)
{
  int sum = summa, tmp;
  for (int i = 5; i >= 0; i--){
    if (sum >= nominal[i]) 
    {
      
      curr [i] = sum / nominal[i];
      sum -= nominal[i] * curr[i] ;
      if (banknotes[i] < curr[i]) 
      {
        tmp = curr[i] - banknotes[i];
        curr[i] -= tmp;
        tmp *= nominal[i];
        sum += tmp;
        banknotes[i] = 0;
      } else {
        banknotes[i] -= curr[i];
      }
    }else curr[i] = 0;
  }
  if (sum != 0){ 
    std::cerr << "Неожиданная ошибка!:) sum = " << sum << "\nAllSumm "  << S << std::endl; 
    return false;
  } else return true;
}

bool withdrawal(int nominal[])
{
  std::ifstream file("DeviceStatus.bin" , std::ios::binary);
  if (!file.is_open()){
    std::cerr << "Ошибка открытия файла на чтение из процедуры withdrawal." << std::endl;
    return false;
  }
  int banknotes[6];
  int curr[6];
  file.read((char*)&banknotes, sizeof(int) * 6); 
  file.close();
  int allSumm = 0;
  for (int i = 0; i < 6; i++) allSumm += (banknotes[i] * nominal[i]);
  do {
    std::cout << "Какую сумму вы хотели бы снять? " << std::endl;
    int summ = 0;
    std::cin.clear();
    std::cin.ignore(100,'\n');
    std::cin >> summ;
    if (summ % 100 != 0) std::cout << "Вы указали сумму не кратную 100 рублей. Попробуйте ввести другую сумму." << std::endl; 
    else if (summ <= 0) std::cout << "Вы указали слишком маленькую сумму. Попробуйте ввести другую сумму." << std::endl; 
    else if (summ > allSumm && summ != 0) std::cout << "Недостаточно средств в банкомате." << std::endl;
    else if (summ >= 100 && countSumm(summ, banknotes, curr, nominal, allSumm)) break;
    else {
      std::cout << "Ошибка! Введено не число." << std::endl;
      return false;
      }
  } while (true);
  std::cout << "Ваши деньги:"  << std::endl;
  for (int i = 5 ; i >= 0; i--){
    if (curr[i] != 0) std::cout << nominal[i] << " pуб." << std::setw(4) << curr[i] << " шт." << std::endl;
    
  }
  std::ofstream fileS("DeviceStatus.bin", std::ios::binary);
  if (!fileS.is_open()){
    std::cerr << "Ошибка открытия файла на запись из процедуры withdrawal при записи оставшихся после выдачи банкнот." << std::endl;
    return false;
    }
  fileS.seekp(0);
  fileS.write((char*)&banknotes, sizeof(int) * 6); 
  fileS.close();
  allSumm = 0;
  for (int i = 0; i < 6; i++) allSumm += (banknotes[i] * nominal[i]);
  return true;
}

int main() {
	std::cout << "Bankomate" << "\n\n";
  int nominal[]= {100,200,500,1000,2000,5000};
  std::string dir;
  do
  {
    std::cout << "Input directive: " << std::endl;
    std::cin >> dir;
    if (dir == "+") {
      load();
      break;
    } else if (dir == "-") {
        if (!withdrawal(nominal)) {
          std::cin.clear();
          std::cin.ignore(100,'\n');
        }
        break;
    } else if (dir == "/") { // проверка количества купюр в банкомате
      std::ifstream sh ("DeviceStatus.bin", std::ios::binary);
      if (!sh.is_open()) {
        std::cerr << "Ошибка открытия файла в режиме чтения из процедуры main при проверке количества банкнот." << std::endl;
      }
      int a[6];
      sh.read((char*)&a, sizeof(int) * 6);
      sh.seekg(0);
      for (int i = 0; i < 6; i++)
      {
        std::cout << nominal[i] << "  \t" << a[i] << std::endl;
      }
      sh.close();
    } else if (dir == "*") break; // только для теста при зацикливании программы, "служебный" выход
  } while (true);
  return 0;
}
