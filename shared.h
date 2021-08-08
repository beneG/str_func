
class CMyClass
{
public:
    CMyClass();

    void raiseEvent1();
    void raiseEvent2();
    void raiseEvent3();

private:
    int m_one = 1;
    int m_two = 2;
    int m_three = 3;
};

CMyClass *get_instance();

